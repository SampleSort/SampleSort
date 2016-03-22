/*
 * GatherSortSamplesStrategy.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef GATHERSORTSAMPLESSTRATEGY_H_
#define GATHERSORTSAMPLESSTRATEGY_H_

#include "SortSamplesStrategy.h"

class GatherSortSamplesStrategy : public SortSamplesStrategy {
public:
	GatherSortSamplesStrategy();
	void sortSamples(vector<int> &samples, vector<int> &splitters, SampleSortParams &sampleSortParams);
	virtual ~GatherSortSamplesStrategy();
};

#endif /* GATHERSORTSAMPLESSTRATEGY_H_ */
