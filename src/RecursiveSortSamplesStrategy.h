/*
 * RecursiveSortSamplesStrategy.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef RECURSIVESORTSAMPLESSTRATEGY_H_
#define RECURSIVESORTSAMPLESSTRATEGY_H_

#include "SortSamplesStrategy.h"

using namespace std;

class RecursiveSortSamplesStrategy : public SortSamplesStrategy<int> {
private:
	const int threshold;

public:
	RecursiveSortSamplesStrategy(int threshold);
	void sortSamples(vector<int> &samples, vector<int> &splitters, SampleSortParams &sampleSortParams);
	virtual ~RecursiveSortSamplesStrategy();
};

#endif /* RECURSIVESORTSAMPLESSTRATEGY_H_ */
