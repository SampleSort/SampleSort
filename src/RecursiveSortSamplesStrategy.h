/*
 * RecursiveSortSamplesStrategy.h
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#ifndef RECURSIVESORTSAMPLESSTRATEGY_H_
#define RECURSIVESORTSAMPLESSTRATEGY_H_

#include "SortSamplesStrategy.h"
#include "GatherSortSamplesStrategy.h"
#include "BinaryTreePrefixSum.h"
#include "mpi.h"
#include "SampleSort.h"
#include "Debug.h"
#include <exception>
#include <cmath>

using namespace std;
using namespace MPI;

template<typename T>
class RecursiveSortSamplesStrategy: public SortSamplesStrategy<T> {
private:
	const int threshold;

public:
	RecursiveSortSamplesStrategy(int threshold) :
			SortSamplesStrategy<T>(),
			threshold(threshold) {
	}

	void sortSamples(vector<T> &samples, vector<T> &splitters,
			SampleSortParams &p) {
		if (samples.size() <= threshold) {
			GatherSortSamplesStrategy<T> gsss;
			gsss.sortSamples(samples, splitters, p);
		} else {
			const int globalSampleCount = p.sampleSize * p.mpiSize;
			SampleSortParams ssp(p);
			SampleSort<T> recurse(ssp, *this);
			vector<T> sortedSamples;
			recurse.sort(samples, sortedSamples, globalSampleCount);

			DEBUG("=======================================================================");

			BinaryTreePrefixSum prefixSum;
			const int offset = prefixSum.prefix_sum(sortedSamples.size());
			DEBUG("Finished prefix sum");

			const int overlap = offset % p.sampleSize;
			const int splitterOffset = offset / p.sampleSize;
			vector<int> localSplitters;
			localSplitters.reserve(sortedSamples.size() / p.sampleSize + 1);
			int limit = sortedSamples.size();

			// The globally last element is not a splitter!
			if (offset + limit == globalSampleCount) {
				limit--;
			}

			for (int j = p.sampleSize - overlap - 1; j < limit;
					j += p.sampleSize) {
				localSplitters.push_back(sortedSamples[j]);
			}

			int *bucketSizes = new int[p.mpiSize];
			const int numberOfLocalSplitters = localSplitters.size() * sizeof(T);

			DEBUG("Pregather");
			// Exchange bucket sizes. Receive how many elements we receive from every other PE.
			COMM_WORLD.Allgather(&numberOfLocalSplitters, 1, MPI::INT,
					bucketSizes, 1, MPI::INT);
			DEBUG("Postgather");

			splitters.resize(max(p.mpiSize - 1, 1));
			int *recPositions = new int[p.mpiSize];

			// Calculate the offsets in the received data buffer for every PE.
			recPositions[0] = 0;
			int allBucketsSize = bucketSizes[0];
			for (int i = 1; i < p.mpiSize; i++) {
				recPositions[i] = bucketSizes[i - 1] + recPositions[i - 1];
				allBucketsSize += bucketSizes[i];
			}

			// Has calculated how much data from which node is received at which position in our receiver array.

			DEBUG("Preallgather");
			// Filling the splitters array.
			COMM_WORLD.Allgatherv(localSplitters.data(), localSplitters.size() * sizeof(T),
					MPI::BYTE, splitters.data(), bucketSizes, recPositions,
					MPI::BYTE);
			DEBUG("Postallgather");

			delete bucketSizes;
			delete recPositions;
		}

		DEBUG("Exit recursive sample sorting");
	}

	virtual ~RecursiveSortSamplesStrategy() {}
};

#endif /* RECURSIVESORTSAMPLESSTRATEGY_H_ */
