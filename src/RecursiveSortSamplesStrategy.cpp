/*
 * RecursiveSortSamplesStrategy.cpp
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#include "RecursiveSortSamplesStrategy.h"
#include "GatherSortSamplesStrategy.h"
#include "mpi.h"
#include "SampleSort.h"
#include <exception>

using namespace std;
using namespace MPI;

RecursiveSortSamplesStrategy::RecursiveSortSamplesStrategy(int threshold) :
		threshold(threshold) {
}

void RecursiveSortSamplesStrategy::sortSamples(vector<int> &samples,
		vector<int> &splitters, SampleSortParams &p) {
	if (samples.size() <= threshold) {
		GatherSortSamplesStrategy gsss;
		gsss.sortSamples(samples, splitters, p);
	} else {
		SampleSortParams ssp(p);
		SampleSort recurse(ssp, *this);
		vector<int> sortedSamples;
		recurse.sort(samples, sortedSamples);

		if (sortedSamples.empty()) {
			throw runtime_error("Cannot draw sample, sorted array is empty!");
		}

		int splitter = sortedSamples[sortedSamples.size() - 1];
	}
}

RecursiveSortSamplesStrategy::~RecursiveSortSamplesStrategy() {
	// TODO Auto-generated destructor stub
}
