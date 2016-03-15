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

SampleSort::SampleSort(int mpiRank, int mpiSize, bool presortLocalData, int sampleSize) :
		presortLocalData(presortLocalData),
		sampleSize(sampleSize),
		mpiRank(mpiRank),
		mpiSize(mpiSize)
{
}

vector<int> SampleSort::sort(vector<int> &data) {
	if (presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<int> samples;
	vector<int> positions;
	vector<int> splitter(mpiSize - 1);

	drawSamples(data, samples);
	sortSamples(samples, splitter);
	partitionData(data, splitter, positions);

	return shareData(data, positions);
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

void SampleSort::sortSamples(vector<int> &samples, vector<int> &splitter) {
	DEBUG("Enter sortSamples");

	int *receiveBuffer = 0;
	int receiveSize = sampleSize * mpiSize;

	if (mpiRank == CUSTOM_MPI_ROOT) {
		receiveBuffer = new int[receiveSize];

		for (int i = 0; i < receiveSize; i++) {
			receiveBuffer[i] = 0;
		}
	}

	COMM_WORLD.Gather(samples.data(), samples.size(), MPI::INT, receiveBuffer, samples.size(), MPI::INT, CUSTOM_MPI_ROOT);

	if (mpiRank == CUSTOM_MPI_ROOT) {
		std::sort(receiveBuffer, receiveBuffer + receiveSize);

		for (int i = 0; i < mpiSize - 1; i++) {
			splitter[i] = receiveBuffer[(i + 1) * sampleSize - 1];
		}

		delete receiveBuffer;
	}

	COMM_WORLD.Bcast(splitter.data(), mpiSize - 1, MPI::INT, CUSTOM_MPI_ROOT);

	DEBUG("Exit sortSamples");
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

vector<int> SampleSort::shareData(vector<int> &data, vector<int> &positions) {
	DEBUG("Enter shareData");
	DEBUGV(positions.size())

	int *bucketSizes = new int[mpiSize];
	int *recBucketSizes = new int[mpiSize];

	for (int i = 0; i < mpiSize - 1; i++) {
		bucketSizes[i] = positions[i + 1] - positions[i];

		if (i == 5) {
		DEBUGV(positions[i])
		DEBUGV(bucketSizes[i])}
	}

	bucketSizes[mpiSize - 1] = data.size() - positions[mpiSize - 1];

	COMM_WORLD.Alltoall(bucketSizes, 1, MPI::INT, recBucketSizes, 1, MPI::INT);

	int receiveSize = 0;

	for (int i = 0; i < mpiSize; i++) {
		receiveSize += recBucketSizes[i];
	}

	DEBUGV(receiveSize)

	int *receivedData = new int[receiveSize];
	int *recPositions = new int[mpiSize];

	recPositions[0] = 0;
	for (int i = 1; i < mpiSize; i++) {
		recPositions[i] = recBucketSizes[i - 1] + recPositions[i - 1];
	}

	COMM_WORLD.Alltoallv(data.data(), bucketSizes, positions.data(), MPI::INT, receivedData, recBucketSizes, recPositions, MPI::INT);

	delete bucketSizes;
	delete recBucketSizes;
	delete recPositions;

	std::sort(receivedData, receivedData + receiveSize);

	vector<int> result(receivedData, receivedData + receiveSize);
	delete receivedData;

	DEBUG("Exit shareData");
	return result;
}

SampleSort::~SampleSort() {
	// TODO Auto-generated destructor stub
}

