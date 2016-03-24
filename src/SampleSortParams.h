/*
 * SampleSortParams.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORTPARAMS_H_
#define SAMPLESORTPARAMS_H_

#include "SampleSizeStrategy.h"

class SampleSortParams {
public:
	const int mpiRank;
	const int mpiSize;
	const int mpiRoot;
	const bool presortLocalData;
	SampleSizeStrategy &sampleSizeStrategy;
	int sampleSize;

public:
	SampleSortParams(SampleSortParams &sampleSortParams);
	SampleSortParams(int mpiRank, int mpiSize, int mpiRoot, bool presortLocalData, int sampleSize, SampleSizeStrategy &sampleSizeStrategy);
	virtual ~SampleSortParams();

	bool isMPIRoot();
};

#endif /* SAMPLESORTPARAMS_H_ */
