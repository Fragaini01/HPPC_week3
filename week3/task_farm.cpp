/*
  Assignment: Make an MPI task farm. A "task" is a randomly generated integer.
  To "execute" a task, the worker sleeps for the given number of milliseconds.
  The result of a task should be send back from the worker to the master. It
  contains the rank of the worker
*/

#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <array>

// To run an MPI program we always need to include the MPI headers
#include <mpi.h>
using namespace std;

const int NTASKS=5000;  // number of tasks
const int RANDOM_SEED=1234;
const int rest_signal = -999; // signal to rest, job done

// call this function to complete the task. It sleeps for task milliseconds
void task_function(int task)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(task));
}

void master (int nworker) {
    std::array<int, NTASKS> task, result;
    int done{};
    int sent{nworker};
    int worker;

    // set up a random number generator
    std::random_device rd;
    //std::default_random_engine engine(rd());
    std::default_random_engine engine;
    engine.seed(RANDOM_SEED);
    // make a distribution of random integers in the interval [0:30]
    std::uniform_int_distribution<int> distribution(0, 30);

    for (int& t : task) {
        t = distribution(engine);   // set up some "tasks"
    }


    MPI_Request request;
    for(int i{1}; i <= nworker; i++)
        MPI_Isend(&task[i-1], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);

    while(sent < NTASKS) {
        MPI_Recv(&worker, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Isend(&task[sent], 1, MPI_INT, worker, 0, MPI_COMM_WORLD , &request);
        result[done] = worker;
        done++;
        sent++;
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }

    // all sent, time to read and rest
    std::vector<MPI_Request> requests(nworker);
    for(int i {} ; i < nworker; i++) {
        MPI_Recv(&worker, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Isend(&rest_signal, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, &requests[i]);
        result[done] = worker;
        done++;
    }
    
    // Print out a status on how many tasks were completed by each worker
    int workdone = 0;
    for (int worker = 0; worker <= nworker; worker++)
    {
        int tasksdone = 0; 
        for (int itask=1; itask<NTASKS; itask++)
        if (result[itask]==worker) {
            tasksdone++;
            workdone ++;
        }
        std::cout << "Master: Worker " << worker << " solved " << tasksdone << 
                    " tasks\n";
    }
                cout<<"total taskes done " <<workdone <<"\n";
                cout<<"total tasks " <<NTASKS <<"\n";  
                
    MPI_Waitall(nworker, requests.data(), MPI_STATUSES_IGNORE); 
}



void worker (int rank) {
    while(true){
    int task ;
    MPI_Recv(&task, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive a task from the master
    if (task == rest_signal) {
        std::cout<<"Time to rest for worker " <<rank <<"\n";
        break;
    }
    else task_function(task);     // execute the task
    MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // send the result back
    }
    /*
    IMPLEMENT HERE THE CODE FOR THE WORKER
    Use a call to "task_function" to complete a task
    */
}

int main(int argc, char *argv[]) {
    
    int nrank, rank;
    
    MPI_Init(&argc, &argv);                // set up MPI
    MPI_Comm_size(MPI_COMM_WORLD, &nrank); // get the total number of ranks
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // get the rank of this process

    auto start = std::chrono::high_resolution_clock::now();
    if (rank == 0)       // rank 0 is the master
        master(nrank-1); // there is nrank-1 worker processes
    else                 // ranks in [1:nrank] are workers
        worker(rank);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    if (rank == 0)
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    MPI_Finalize();      // shutdown MPI
}
