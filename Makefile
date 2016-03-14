CXX = ~/bin/mpic++

OPTFLAGS = -O3
CXXFLAGS = $(OPTFLAGS) -g
INCLUDEPATH = ~/include/

all: samplesort

samplesort: src/Start.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEPATH) -o SampleSort src/Start.cpp src/SampleSort.cpp

clean:
	rm SampleSort

