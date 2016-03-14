CXX = ~/bin/mpic++

OPTFLAGS = -O3
CXXFLAGS = $(OPTFLAGS) -g -std=c++11
INCLUDEPATH = ~/include/

all: samplesort

samplesort: src/Start.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEPATH) -o SampleSort src/Start.cpp src/SampleSort.cpp

clean:
	rm -f SampleSort

