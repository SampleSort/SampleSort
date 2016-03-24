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
#include "Debug.h"

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
		DEBUG("Sorting samples with gathering");
		int receiveSize = p.sampleSize * p.mpiSize;
		vector<T> receiveBuffer;

		if (p.isMPIRoot()) {
			receiveBuffer.resize(receiveSize);
		}

		DEBUG("Gathering");
		COMM_WORLD.Gather(samples.data(), samples.size() * sizeof(T), MPI::BYTE,
				receiveBuffer.data(), samples.size() * sizeof(T), MPI::BYTE, p.mpiRoot);
		DEBUG("Gathered");

		if (p.isMPIRoot()) {
			std::sort(receiveBuffer.begin(), receiveBuffer.end());
			for (int i = 0; i < p.mpiSize - 1; i++) {
				splitters[i] = receiveBuffer[(i + 1) * p.sampleSize - 1];
			}
		}

		DEBUGV(sizeof(T));
		DEBUGV(splitters.size());
		DEBUG("Bcasting");
		COMM_WORLD.Bcast(splitters.data(), (p.mpiSize - 1) * sizeof(T), MPI::BYTE, p.mpiRoot);
		DEBUG("Bcasted");
	}

	virtual ~GatherSortSamplesStrategy() {}
};

#endif /* GATHERSORTSAMPLESSTRATEGY_H_ */
