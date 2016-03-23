/*
 * SortSamplesStrategy.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef SORTSAMPLESSTRATEGY_H_
#define SORTSAMPLESSTRATEGY_H_

#include "SampleSortParams.h"

#include <vector>

using namespace std;

template<typename T>
class SortSamplesStrategy {
public:
	virtual void sortSamples(vector<T> &samples, vector<T> &splitters, SampleSortParams &sampleSortParams) = 0;
	virtual ~SortSamplesStrategy() {}
};



#endif /* SORTSAMPLESSTRATEGY_H_ */
