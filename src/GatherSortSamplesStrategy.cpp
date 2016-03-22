/*
 * GatherSortSamplesStrategy.cpp
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#include "GatherSortSamplesStrategy.h"

#include <algorithm>

using namespace std;
using namespace MPI;

GatherSortSamplesStrategy::GatherSortSamplesStrategy() : SortSamplesStrategy() {
	// TODO Auto-generated constructor stub

}

void GatherSortSamplesStrategy::sortSamples(vector<int> &samples,
		vector<int> &splitters, int mpiRoot, int sampleSize) {
	const int mpiRank = COMM_WORLD.Get_rank();
	const int mpiSize = COMM_WORLD.Get_size();

	int *receiveBuffer = 0;
	int receiveSize = sampleSize * mpiSize;

	if (mpiRank == mpiRoot) {
		receiveBuffer = new int[receiveSize];
		for (int i = 0; i < receiveSize; i++) {
			receiveBuffer[i] = 0;
		}
	}

	COMM_WORLD.Gather(samples.data(), samples.size(), MPI::INT, receiveBuffer,
			samples.size(), MPI::INT, mpiRoot);
	if (mpiRank == mpiRoot) {
		std::sort(receiveBuffer, receiveBuffer + receiveSize);
		for (int i = 0; i < mpiSize - 1; i++) {
			splitters[i] = receiveBuffer[(i + 1) * sampleSize - 1];
		}
		delete receiveBuffer;
	}

	COMM_WORLD.Bcast(splitters.data(), mpiSize - 1, MPI::INT, mpiRoot);
}

GatherSortSamplesStrategy::~GatherSortSamplesStrategy() {
	// TODO Auto-generated destructor stub
}

