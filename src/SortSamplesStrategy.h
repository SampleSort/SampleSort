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

class SortSamplesStrategy {
public:
	virtual void sortSamples(vector<int> &samples, vector<int> &splitters, SampleSortParams &sampleSortParams) = 0;
	virtual ~SortSamplesStrategy() {}
};



#endif /* SORTSAMPLESSTRATEGY_H_ */
