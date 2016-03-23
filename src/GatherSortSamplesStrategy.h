/*
 * GatherSortSamplesStrategy.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef GATHERSORTSAMPLESSTRATEGY_H_
#define GATHERSORTSAMPLESSTRATEGY_H_

#include "SortSamplesStrategy.h"
#include "mpi.h"

#include <algorithm>

using namespace std;
using namespace MPI;

template<typename T>
class GatherSortSamplesStrategy: public SortSamplesStrategy<T> {
public:
	GatherSortSamplesStrategy() :
			SortSamplesStrategy<T>() {
	}

	void sortSamples(vector<T> &samples, vector<T> &splitters,
			SampleSortParams &p) {
		int *receiveBuffer = 0;
		int receiveSize = p.sampleSize * p.mpiSize;

		if (p.isMPIRoot()) {
			receiveBuffer = new int[receiveSize];
			for (int i = 0; i < receiveSize; i++) {
				receiveBuffer[i] = 0;
			}
		}

		COMM_WORLD.Gather(samples.data(), samples.size() * sizeof(T), MPI::BYTE,
				receiveBuffer, samples.size() * sizeof(T), MPI::BYTE, p.mpiRoot);
		if (p.isMPIRoot()) {
			std::sort(receiveBuffer, receiveBuffer + receiveSize);
			for (int i = 0; i < p.mpiSize - 1; i++) {
				splitters[i] = receiveBuffer[(i + 1) * p.sampleSize - 1];
			}
			delete receiveBuffer;
		}

		COMM_WORLD.Bcast(splitters.data(), (p.mpiSize - 1) * sizeof(T), MPI::BYTE, p.mpiRoot);
	}

	virtual ~GatherSortSamplesStrategy() {}
};

#endif /* GATHERSORTSAMPLESSTRATEGY_H_ */
