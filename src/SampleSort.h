/*
 * SampleSort.h
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORT_H_
#define SAMPLESORT_H_

#include "Debug.h"

#include <vector>

using namespace std;

class SampleSort {
private:
	const int mpiRank;
	const int mpiSize;
	const bool presortLocalData;
	const int sampleSize;

public:
	SampleSort(int mpiRank, int mpiSize, bool presortLocalData, int sampleSize);
	virtual ~SampleSort();


	void sort(vector<int> &data, vector<int> &sortedData);
	void drawSamples(vector<int> &data, vector<int> &samples);
	void sortSamples(vector<int> &samples, vector<int> &splitter);
	void partitionData(vector<int> &data, vector<int> &splitter, vector<int> &positions);
	void shareData(vector<int> &data, vector<int> &positions, vector<int> &receivedData);
	void sortData(vector<int> &receivedData);
};

#endif /* SAMPLESORT_H_ */
