/*
 * SampleSort.cpp
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#include "mpi.h"
#include "SampleSort.h"
#include "Random.h"

#include <algorithm>
#include <iomanip>
#include <random>
#include <iterator>
#include <cmath>

using namespace std;
using namespace MPI;

const int CUSTOM_MPI_ROOT = 0;

SampleSort::SampleSort(SampleSort &sampleSort) :
		p(sampleSort.p), sortSamplesStrategy(sampleSort.sortSamplesStrategy) {

}

SampleSort::SampleSort(SampleSortParams &p,
		SortSamplesStrategy &sortSamplesStrategy) :
		p(p), sortSamplesStrategy(sortSamplesStrategy) {

}

void SampleSort::determineSampleSize(int dataSize) {
	if (p.isMPIRoot()) {
		p.sampleSize = min((int) log2(dataSize * p.mpiSize) / p.mpiSize,
				dataSize);
	}

	COMM_WORLD.Bcast(&(p.sampleSize), 1, MPI::INT, p.mpiRoot);
}

void SampleSort::sort(vector<int> &data, vector<int> &sortedData) {
	if (p.presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<int> samples;
	vector<int> positions;
	vector<int> splitter(p.mpiSize - 1);

	determineSampleSize(data.size());
	DEBUGV(p.sampleSize);
	drawSamples(data, samples);
	sortSamplesStrategy.sortSamples(samples, splitter, p);
	partitionData(data, splitter, positions);
	shareData(data, positions, sortedData);
	sortData(sortedData);
}

void SampleSort::drawSamples(vector<int> &data, vector<int> &samples) {
	default_random_engine randomGenerator(getSeed() * (p.mpiRank + 5));
	uniform_int_distribution<int> randomDistribution(0, data.size() - 1);

	while (samples.size() < p.sampleSize) {
		int randomValue = randomDistribution(randomGenerator);
		samples.push_back(data[randomValue]);
		DEBUGV(randomValue)
	}
}

void SampleSort::partitionData(vector<int> &data, vector<int> &splitter,
		vector<int> &positions) {
	DEBUG("Enter partitionData");

// BINARY SEARCH FOR SPLITTER POSITIONS
	vector<int>::iterator first = data.begin();

	positions.push_back(0);
	for (int i = 0; i < p.mpiSize - 1; i++) {
		first = lower_bound(first, data.end(), splitter[i]);
		positions.push_back(first - data.begin());
	}

	DEBUG("Exit partitionData");
}

void SampleSort::shareData(vector<int> &data, vector<int> &positions,
		vector<int> &receivedData) {
	DEBUG("Enter shareData");
	DEBUGV(positions.size())

	int *bucketSizes = new int[p.mpiSize];
	int *recBucketSizes = new int[p.mpiSize];

	// Calculate our bucket sizes
	for (int i = 0; i < p.mpiSize - 1; i++) {
		bucketSizes[i] = positions[i + 1] - positions[i];
	}

	bucketSizes[p.mpiSize - 1] = data.size() - positions[p.mpiSize - 1];

	// Exchange bucket sizes. Receive how many elements we receive from every other PE.
	COMM_WORLD.Alltoall(bucketSizes, 1, MPI::INT, recBucketSizes, 1, MPI::INT);

	int receiveSize = 0;

	// Count how many elements we receive from all other PEs added up.
	for (int i = 0; i < p.mpiSize; i++) {
		receiveSize += recBucketSizes[i];
	}

	DEBUGV(receiveSize)
	receivedData.resize(receiveSize);
	int *recPositions = new int[p.mpiSize];

	// Calculate the offsets in the received data buffer for every PE.
	recPositions[0] = 0;
	for (int i = 1; i < p.mpiSize; i++) {
		recPositions[i] = recBucketSizes[i - 1] + recPositions[i - 1];
	}

	// Has calculated how much data from which node is received at which position in our receiver array.

	COMM_WORLD.Alltoallv(data.data(), bucketSizes, positions.data(), MPI::INT,
			receivedData.data(), recBucketSizes, recPositions, MPI::INT);

	delete bucketSizes;
	delete recBucketSizes;
	delete recPositions;
	DEBUG("Exit shareData");
}

void SampleSort::sortData(vector<int> &receivedData) {
	std::sort(receivedData.begin(), receivedData.end());
}

SampleSort::~SampleSort() {
// TODO Auto-generated destructor stub
}

