/*
 * SampleSortParams.cpp
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#include "SampleSortParams.h"

SampleSortParams::SampleSortParams(int mpiRank, int mpiSize, int mpiRoot,
		bool presortLocalData, int sampleSize) :
		presortLocalData(presortLocalData), sampleSize(sampleSize), mpiRank(
				mpiRank), mpiSize(mpiSize), mpiRoot(mpiRoot) {
}

bool SampleSortParams::isMPIRoot() {
	return mpiRoot == mpiRank;
}

SampleSortParams::~SampleSortParams() {
	// TODO Auto-generated destructor stub
}
