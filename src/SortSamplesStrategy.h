/*
 * SortSamplesStrategy.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef SORTSAMPLESSTRATEGY_H_
#define SORTSAMPLESSTRATEGY_H_

#include <vector>
#include "mpi.h"

using namespace std;

class SortSamplesStrategy {
public:
	virtual void sortSamples(vector<int> &samples, vector<int> &splitters, int mpiRoot, int sampleSize) = 0;
	virtual ~SortSamplesStrategy() {}
};



#endif /* SORTSAMPLESSTRATEGY_H_ */
