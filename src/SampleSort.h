/*
 * SampleSort.h
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORT_H_
#define SAMPLESORT_H_

#include "mpi.h"
#include <vector>
#include <cstdlib>

using namespace std;

class SampleSort {
private:
	const int mpiRank;
	const int mpiSize;
	const bool presortLocalData;
	const size_t sampleSize;

public:
	SampleSort(int mpiRank, int mpiSize, bool presortLocalData, size_t sampleSize);
	virtual ~SampleSort();


	void sort(vector<int> &data);
	void drawSamples(vector<int> &data, vector<int> &samples);
	void sortSamples(vector<int> &samples);
};

#endif /* SAMPLESORT_H_ */
