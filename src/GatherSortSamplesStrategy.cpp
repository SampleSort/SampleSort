/*
 * GatherSortSamplesStrategy.cpp
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#include "GatherSortSamplesStrategy.h"
#include "mpi.h"

#include <algorithm>

using namespace std;
using namespace MPI;

GatherSortSamplesStrategy::GatherSortSamplesStrategy() : SortSamplesStrategy() {
	// TODO Auto-generated constructor stub

}

void GatherSortSamplesStrategy::sortSamples(vector<int> &samples,
		vector<int> &splitters, SampleSortParams &p) {
	int *receiveBuffer = 0;
	int receiveSize = p.sampleSize * p.mpiSize;

	if (p.isMPIRoot()) {
		receiveBuffer = new int[receiveSize];
		for (int i = 0; i < receiveSize; i++) {
			receiveBuffer[i] = 0;
		}
	}

	COMM_WORLD.Gather(samples.data(), samples.size(), MPI::INT, receiveBuffer,
			samples.size(), MPI::INT, p.mpiRoot);
	if (p.isMPIRoot()) {
		std::sort(receiveBuffer, receiveBuffer + receiveSize);
		for (int i = 0; i < p.mpiSize - 1; i++) {
			splitters[i] = receiveBuffer[(i + 1) * p.sampleSize - 1];
		}
		delete receiveBuffer;
	}

	COMM_WORLD.Bcast(splitters.data(), p.mpiSize - 1, MPI::INT, p.mpiRoot);
}

GatherSortSamplesStrategy::~GatherSortSamplesStrategy() {
	// TODO Auto-generated destructor stub
}

