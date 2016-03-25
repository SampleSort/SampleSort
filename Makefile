CXX = mpic++

OPTFLAGS = -O3
CXXFLAGS = $(OPTFLAGS) -g -std=c++11
INCLUDEPATH = ~/include/
_OBJECTS = SampleSort.o Random.o SampleSortParams.o BinaryTreePrefixSum.o LogSampleSizeStrategy.o RootSampleSizeStrategy.o
OBJECTS = $(patsubst %,build/%,$(_OBJECTS))

_SSOBJECTS = $(_OBJECTS) Start.o
SSOBJECTS = $(patsubst %,build/%,$(_SSOBJECTS))

_TPOBJECTS = $(_OBJECTS) TestPrefix.o
TPOBJECTS = $(patsubst %,build/%,$(_TPOBJECTS))

_ALLOBJECTS = $(_OBJECTS) Start.o TestPrefix.o
ALLOBJECTS = $(patsubst %,build/%,$(_ALLOBJECTS))

.PHONY: all

all: samplesort testprefix

samplesort: $(SSOBJECTS)
	export PATH=$$PATH:~/bin/; $(CXX) $(SSOBJECTS) -o SampleSort
	
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
