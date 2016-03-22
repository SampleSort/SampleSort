/*
 * SampleSort.h
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORT_H_
#define SAMPLESORT_H_

#include "Debug.h"
#include "SortSamplesStrategy.h"
#include "SampleSortParams.h"

#include <vector>

using namespace std;

class SampleSort {
private:
	SampleSortParams p;

	SortSamplesStrategy &sortSamplesStrategy;

public:
	SampleSort(SampleSort &sampleSort);
	SampleSort(SampleSortParams &p, SortSamplesStrategy &sortSamplesStrategy);
	virtual ~SampleSort();

	void determineSampleSize(int dataSize);

	void sort(vector<int> &data, vector<int> &sortedData);
	void drawSamples(vector<int> &data, vector<int> &samples);
	void sortSamples(vector<int> &samples, vector<int> &splitter);
	void partitionData(vector<int> &data, vector<int> &splitter, vector<int> &positions);
	void shareData(vector<int> &data, vector<int> &positions, vector<int> &receivedData);
	void sortData(vector<int> &receivedData);
};

#endif /* SAMPLESORT_H_ */
