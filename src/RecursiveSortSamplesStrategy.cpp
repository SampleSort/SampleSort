/*
 * RecursiveSortSamplesStrategy.cpp
 *
 *  Created on: 22.03.2016
 *      Author: s_sschmi
 */

#include "RecursiveSortSamplesStrategy.h"
#include "GatherSortSamplesStrategy.h"
#include "BinaryTreePrefixSum.h"
#include "mpi.h"
#include "SampleSort.h"
#include "Debug.h"
#include <exception>
#include <cmath>

using namespace std;
using namespace MPI;

RecursiveSortSamplesStrategy::RecursiveSortSamplesStrategy(int threshold) :
		threshold(threshold) {
}

void RecursiveSortSamplesStrategy::sortSamples(vector<int> &samples,
		vector<int> &splitters, SampleSortParams &p) {
	if (samples.size() <= threshold) {
		GatherSortSamplesStrategy gsss;
		gsss.sortSamples(samples, splitters, p);
	} else {
		SampleSortParams ssp(p);
		SampleSort<int> recurse(ssp, *this);
		vector<int> sortedSamples;
		recurse.sort(samples, sortedSamples);

		DEBUG("=======================================================================");

		BinaryTreePrefixSum prefixSum;
		const int offset = prefixSum.prefix_sum(sortedSamples.size());
		DEBUG("Finished prefix sum");



		const int overlap = offset % p.sampleSize;
		const int splitterOffset = offset / p.sampleSize; // TODO !!!
		vector<int> localSplitters;
		localSplitters.reserve(sortedSamples.size() / p.sampleSize + 1);

		for (int j = p.sampleSize - overlap - 1; j < sortedSamples.size(); j +=
				p.sampleSize) {
			localSplitters.push_back(sortedSamples[j]);
		}

		int *bucketSizes = new int[p.mpiSize];
		const int numberOfLocalSplitters = localSplitters.size();
		int splitterno = numberOfLocalSplitters;

		DEBUG("Pregather");
		// Exchange bucket sizes. Receive how many elements we receive from every other PE.
		COMM_WORLD.Allgather(&numberOfLocalSplitters, 1, MPI::INT, bucketSizes,
				1, MPI::INT);
		DEBUG("Postgather");

		splitters.resize(max(p.mpiSize - 1, 1));
		int *recPositions = new int[p.mpiSize];

		// Calculate the offsets in the received data buffer for every PE.
		recPositions[0] = 0;
		for (int i = 1; i < p.mpiSize; i++) {
			recPositions[i] = bucketSizes[i - 1] + recPositions[i - 1];
		}

		// Has calculated how much data from which node is received at which position in our receiver array.

		DEBUGV(splitters.size());
		DEBUGV(splitters.capacity());
		DEBUG("Preallgather");
		// Filling the splitters array.
		COMM_WORLD.Allgatherv(localSplitters.data(), localSplitters.size(),
				MPI::INT, splitters.data(), bucketSizes, recPositions,
				MPI::INT);
		DEBUG("Postallgather");

		delete bucketSizes;
		delete recPositions;
	}
}

RecursiveSortSamplesStrategy::~RecursiveSortSamplesStrategy() {
	// TODO Auto-generated destructor stub
}
