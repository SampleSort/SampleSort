/*
 * SampleSort.h
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORT_H_
#define SAMPLESORT_H_

#include "mpi.h"
#include "Debug.h"

#include <vector>

using namespace std;

class SampleSort {
private:
	const int mpiRank;
	const int mpiSize;
	const bool presortLocalData;
	const int sampleSize;

	int *splitter;

public:
	SampleSort(int mpiRank, int mpiSize, bool presortLocalData, int sampleSize);
	virtual ~SampleSort();


	vector<int> sort(vector<int> &data);
	void drawSamples(vector<int> &data, vector<int> &samples);
	void sortSamples(vector<int> &samples);
	void partitionData(vector<int> &data, vector<int> &positions);
	vector<int> shareData(vector<int> &data, vector<int> &positions);
};

#endif /* SAMPLESORT_H_ */
