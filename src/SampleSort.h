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

template <typename T>
class SampleSort {
protected:
	SampleSortParams p;

	SortSamplesStrategy<T> &sortSamplesStrategy;

public:
	SampleSort(SampleSort &sampleSort);
	SampleSort(SampleSortParams &p, SortSamplesStrategy<T> &sortSamplesStrategy);
	virtual ~SampleSort();

	void sort(vector<T> &data, vector<T> &sortedData, int globalDataSize);
	void drawSamples(vector<T> &data, vector<T> &samples);
	void sortSamples(vector<T> &samples, vector<T> &splitter);
	virtual void partitionData(vector<T> &data, vector<T> &splitter, vector<int> &positions);
	void shareData(vector<T> &data, vector<int> &positions, vector<T> &receivedData);
	virtual void sortData(vector<T> &receivedData);
};

template class SampleSort<char>;
template class SampleSort<short>;
template class SampleSort<int>;
template class SampleSort<long>;
template class SampleSort<long long>;

template class SampleSort<unsigned char>;
template class SampleSort<unsigned short>;
template class SampleSort<unsigned int>;
template class SampleSort<unsigned long>;
template class SampleSort<unsigned long long>;

template class SampleSort<float>;
template class SampleSort<double>;
template class SampleSort<long double>;

#endif /* SAMPLESORT_H_ */
