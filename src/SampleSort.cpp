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


using namespace std;
using namespace MPI;

const int CUSTOM_MPI_ROOT = 0;

SampleSort::SampleSort(int mpiRank, int mpiSize, bool presortLocalData, int sampleSize, SortSamplesStrategy &sortSamplesStrategy) :
		presortLocalData(presortLocalData),
		sampleSize(sampleSize),
		mpiRank(mpiRank),
		mpiSize(mpiSize),
		sortSamplesStrategy(sortSamplesStrategy)
{
}

void SampleSort::sort(vector<int> &data, vector<int> &sortedData) {
	if (presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<int> samples;
	vector<int> positions;
	vector<int> splitter(mpiSize - 1);

	drawSamples(data, samples);
	sortSamplesStrategy.sortSamples(samples, splitter, CUSTOM_MPI_ROOT, sampleSize);
	partitionData(data, splitter, positions);
	shareData(data, positions, sortedData);
	sortData(sortedData);
}

void SampleSort::drawSamples(vector<int> &data, vector<int> &samples) {
	default_random_engine randomGenerator(getSeed() * (mpiRank + 5));
	uniform_int_distribution<int> randomDistribution(0, data.size() - 1);

	while (samples.size() < sampleSize) {
		int randomValue = randomDistribution(randomGenerator);
		samples.push_back(data[randomValue]);
		DEBUGV(randomValue)
	}
}

void SampleSort::partitionData(vector<int> &data, vector<int> &splitter, vector<int> &positions) {
	DEBUG("Enter partitionData");

	// BINARY SEARCH FOR SPLITTER POSITIONS
	vector<int>::iterator first = data.begin();

	positions.push_back(0);
	for (int i = 0; i < mpiSize - 1; i++) {
		first = lower_bound(first, data.end(), splitter[i]);
		positions.push_back(first - data.begin());
	}

	DEBUG("Exit partitionData");
}

void SampleSort::shareData(vector<int> &data, vector<int> &positions, vector<int> &receivedData) {
	DEBUG("Enter shareData");
	DEBUGV(positions.size())

	int *bucketSizes = new int[mpiSize];
	int *recBucketSizes = new int[mpiSize];

	for (int i = 0; i < mpiSize - 1; i++) {
		bucketSizes[i] = positions[i + 1] - positions[i];
	}

	bucketSizes[mpiSize - 1] = data.size() - positions[mpiSize - 1];

	COMM_WORLD.Alltoall(bucketSizes, 1, MPI::INT, recBucketSizes, 1, MPI::INT);

	int receiveSize = 0;

	for (int i = 0; i < mpiSize; i++) {
		receiveSize += recBucketSizes[i];
	}

	DEBUGV(receiveSize)
	receivedData.resize(receiveSize);
	int *recPositions = new int[mpiSize];

	recPositions[0] = 0;
	for (int i = 1; i < mpiSize; i++) {
		recPositions[i] = recBucketSizes[i - 1] + recPositions[i - 1];
	}

	// Has calculated how much data from which node is received at which position in our receiver array.

	COMM_WORLD.Alltoallv(data.data(), bucketSizes, positions.data(), MPI::INT, receivedData.data(), recBucketSizes, recPositions, MPI::INT);

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

