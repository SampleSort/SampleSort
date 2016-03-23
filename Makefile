CXX = mpic++

OPTFLAGS = -O3
CXXFLAGS = $(OPTFLAGS) -g -std=c++11
INCLUDEPATH = ~/include/
_OBJECTS = Start.o SampleSort.o Random.o GatherSortSamplesStrategy.o RecursiveSortSamplesStrategy.o SampleSortParams.o BinaryTreePrefixSum.o
OBJECTS = $(patsubst %,build/%,$(_OBJECTS))

.PHONY: all

all: samplesort

samplesort: $(OBJECTS)
	export PATH=$$PATH:~/bin/; $(CXX) $(OBJECTS) -o SampleSort

$(OBJECTS): build/%.o: src/%.cpp build/%.d
	export PATH=$$PATH:~/bin/; $(CXX) -c $(CXXFLAGS) -I $(INCLUDEPATH) $< -o $@

build/%.d: src/%.cpp
	export PATH=$$PATH:~/bin/; $(CXX) -MM $(CXXFLAGS) -I $(INCLUDEPATH) $< > $@

clean:
	rm -f SampleSort
	rm -f build/*
