CXX = mpic++

OPTFLAGS = -O3
CXXFLAGS = $(OPTFLAGS) -g -std=c++11
INCLUDEPATH = ~/include/
_OBJECTS = Start.o SampleSort.o Random.o RecursiveSortSamplesStrategy.o SampleSortParams.o BinaryTreePrefixSum.o
OBJECTS = $(patsubst %,build/%,$(_OBJECTS))

_TPOBJECTS = BinaryTreePrefixSum.o TestPrefix.o
TPOBJECTS = $(patsubst %,build/%,$(_TPOBJECTS))

ALLOBJECTS = $(OBJECTS) $(TPOBJECTS)

.PHONY: all

all: samplesort testprefix

samplesort: $(OBJECTS)
	export PATH=$$PATH:~/bin/; $(CXX) $(OBJECTS) -o SampleSort
	
testprefix: $(TPOBJECTS)
	export PATH=$$PATH:~/bin/; $(CXX) $(TPOBJECTS) -o TestPrefix

$(ALLOBJECTS): build/%.o: src/%.cpp build/%.d
	export PATH=$$PATH:~/bin/; $(CXX) -c $(CXXFLAGS) -I $(INCLUDEPATH) $< -o $@

build/%.d: src/%.cpp
	export PATH=$$PATH:~/bin/; $(CXX) -MM $(CXXFLAGS) -I $(INCLUDEPATH) $< > $@

clean:
	rm -f SampleSort
	rm -f TestPrefix
	rm -f build/*
