CXX := mpic++

#######################################################
# Optimization flags are chosen as the last definition.
# Comment out using "#" at the begining of the line or rearrange according to your needs.
#
# Fastest executable (-ffast-math removes checking for NaNs and other things)
OPT=-O3

CXXFLAGS := $(OPT) -Wall -Wno-unused-const-variable -std=c++17 -march=native

default: task_farm task_farm_HEP task_farm_HEP_seq

task_farm: task_farm.cpp
	$(CXX) task_farm.cpp $(CXXFLAGS) -o task_farm

task_farm_HEP: task_farm_HEP.cpp
	$(CXX) task_farm_HEP.cpp $(CXXFLAGS) -o task_farm_HEP

task_farm_HEP_seq: task_farm_HEP_seq.cpp
	$(CXX) task_farm_HEP_seq.cpp $(CXXFLAGS) -o task_farm_HEP_seq

clean:
	rm -f task_farm task_farm_HEP task_farm_HEP_seq

exercise2: task_farm_HEP task_farm_HEP_seq
	for i in 3 4 5 6 7 8 9 10; do \
	j=$$(($$i-1)); \
	echo "Running for $$j workers"; \
	mpirun -np $$i --oversubscribe ./task_farm_HEP > data/task_farm_HEP.v2.$$j.workers.txt ;\
	done

exercise1: task_farm task_farm_HEP task_farm_HEP_seq
	for i in 3 4 5 6 7 8 9 10; do \
	j=$$(($$i-1)); \
	echo "Running for $$j workers"; \
	mpirun -np $$i --oversubscribe ./task_farm > data/task_farm.v3.$$j.workers.txt ;\
	done