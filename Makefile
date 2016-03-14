CC = ~/bin/mpicc
CXX = ~/bin/mpic++

OPTFLAGS = -O3
CFLAGS = $(OPTFLAGS) -g
CXXFLAGS = $(OPTFLAGS) -g
FFLAGS = $(OPTFLAGS) -g

all: samplesort

samplesort: src/Start.cpp
	$(CC) $(CFLAGS) -o SampleSort src/Start.cpp src/SampleSort.cpp

clean:
	rm SampleSort

