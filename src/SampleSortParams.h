/*
 * SampleSortParams.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORTPARAMS_H_
#define SAMPLESORTPARAMS_H_

class SampleSortParams {
public:
	const int mpiRank;
	const int mpiSize;
	const int mpiRoot;
	const bool presortLocalData;
	const int sampleSize;

public:
	SampleSortParams(int mpiRank, int mpiSize, int mpiRoot, bool presortLocalData, int sampleSize);
	virtual ~SampleSortParams();

	bool isMPIRoot();
};

#endif /* SAMPLESORTPARAMS_H_ */
