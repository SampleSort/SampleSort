/*
 * RecursiveSortSamplesStrategy.cpp
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#include "RecursiveSortSamplesStrategy.h"
#include "mpi.h"
#include "SampleSort.h"

RecursiveSortSamplesStrategy::RecursiveSortSamplesStrategy() {
	// TODO Auto-generated constructor stub

}

void RecursiveSortSamplesStrategy::sortSamples(vector<int> &samples, vector<int> &splitters, SampleSortParams &p) {
	RecursiveSortSamplesStrategy rsss;
	SampleSort recurse(p, rsss);
}

RecursiveSortSamplesStrategy::~RecursiveSortSamplesStrategy() {
	// TODO Auto-generated destructor stub
}

