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

template<typename T>
SampleSort<T>::SampleSort(SampleSort &sampleSort) :
		p(sampleSort.p), sortSamplesStrategy(sampleSort.sortSamplesStrategy) {

}

template<typename T>
SampleSort<T>::SampleSort(SampleSortParams &p,
		SortSamplesStrategy<T> &sortSamplesStrategy) :
		p(p), sortSamplesStrategy(sortSamplesStrategy) {

}

template<typename T>
void SampleSort<T>::determineSampleSize(int dataSize) {
	if (p.isMPIRoot()) {
		p.sampleSize = min((int) log2(dataSize * p.mpiSize) * 10,
				dataSize / 2);
	}

	COMM_WORLD.Bcast(&(p.sampleSize), sizeof(T), MPI::BYTE, p.mpiRoot);
	INFOR(p.sampleSize);
}

template<typename T>
void SampleSort<T>::sort(vector<T> &data, vector<T> &sortedData) {
	if (p.presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<T> samples;
	vector<int> positions;
	vector<T> splitter(p.mpiSize - 1);

	determineSampleSize(data.size());
	//DEBUGV(p.sampleSize);
	drawSamples(data, samples);
	//DEBUG("Drew samples");
	sortSamplesStrategy.sortSamples(samples, splitter, p);
	//DEBUG("Sorted samples");
	partitionData(data, splitter, positions);
	DEBUG("Partitioned data");
	shareData(data, positions, sortedData);
	//DEBUG("Shared data");
	sortData(sortedData);
	DEBUG("Sorted data");
	DEBUGV(sortedData.size());
	DEBUGV(samples.size());
	DEBUGV(samples.capacity());
	DEBUGV(positions.size());
	DEBUGV(positions.capacity());
	DEBUGV(splitter.size());
	DEBUGV(splitter.capacity());
}

template<typename T>
void SampleSort<T>::drawSamples(vector<T> &data, vector<T> &samples) {
	default_random_engine randomGenerator(getSeed() * (p.mpiRank + 5));
	uniform_int_distribution<int> randomDistribution(0, data.size() - 1);

	while (samples.size() < p.sampleSize) {
		int randomValue = randomDistribution(randomGenerator);
		samples.push_back(data[randomValue]);
	}
}

template<typename T>
void SampleSort<T>::partitionData(vector<T> &data, vector<T> &splitter,
		vector<int> &positions) {
// BINARY SEARCH FOR SPLITTER POSITIONS
	typename vector<T>::iterator first = data.begin();

	positions.push_back(0);
	for (int i = 0; i < p.mpiSize - 1; i++) {
		first = lower_bound(first, data.end(), splitter[i]);
		positions.push_back(first - data.begin());
	}
}

template<typename T>
void SampleSort<T>::shareData(vector<T> &data, vector<int> &positions,
		vector<T> &receivedData) {
	int *bucketSizes = new int[p.mpiSize];
	int *recBucketSizes = new int[p.mpiSize];

	// Calculate our bucket sizes
	for (int i = 0; i < p.mpiSize - 1; i++) {
		bucketSizes[i] = sizeof(T) * (positions[i + 1] - positions[i]);
	}

	bucketSizes[p.mpiSize - 1] = sizeof(T) * (data.size() - positions[p.mpiSize - 1]);

	// Exchange bucket sizes. Receive how many elements we receive from every other PE.
	COMM_WORLD.Alltoall(bucketSizes, 1, MPI::INT, recBucketSizes, 1, MPI::INT);

	int receiveSize = 0;

	// Count how many elements we receive from all other PEs added up.
	for (int i = 0; i < p.mpiSize; i++) {
		receiveSize += recBucketSizes[i];
	}

	receivedData.resize(receiveSize / sizeof(T));

	if (receiveSize / sizeof(T) == 0) {
		receivedData.reserve(1);
	}

	int *recPositions = new int[p.mpiSize];

	// Calculate the offsets in the received data buffer for every PE.
	recPositions[0] = 0;
	for (int i = 1; i < p.mpiSize; i++) {
		recPositions[i] = recBucketSizes[i - 1] + recPositions[i - 1];
	}

	// Has calculated how much data from which node is received at which position in our receiver array.

	COMM_WORLD.Alltoallv(data.data(), bucketSizes, positions.data(), MPI::BYTE,
			receivedData.data(), recBucketSizes, recPositions, MPI::BYTE);

	delete bucketSizes;
	delete recBucketSizes;
	delete recPositions;
}

template<typename T>
void SampleSort<T>::sortData(vector<T> &receivedData) {
	std::sort(receivedData.begin(), receivedData.end());
}

template<typename T>
SampleSort<T>::~SampleSort() {
// TODO Auto-generated destructor stub
}

