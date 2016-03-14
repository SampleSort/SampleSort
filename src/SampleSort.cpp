/*
 * SampleSort.cpp
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#include "SampleSort.h"
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace MPI;

const int CUSTOM_MPI_ROOT = 0;

SampleSort::SampleSort(int mpiRank, int mpiSize, bool presortLocalData, size_t sampleSize) :
		presortLocalData(presortLocalData),
		sampleSize(sampleSize),
		mpiRank(mpiRank),
		mpiSize(mpiSize)
{
	splitter = 0;
}

void SampleSort::sort(vector<int> &data) {
	if (presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<int> samples;
	vector<size_t> positions;

	drawSamples(data, samples);
	sortSamples(samples);
	partitionData(data, positions);

	cout << mpiRank << ": positions = ";

	for (int i = 0; i < positions.size(); i++) {
		cout << dec << positions[i] << ",";
	}

	shareData(data, positions);

	cout << endl;
}

void SampleSort::drawSamples(vector<int> &data, vector<int> &samples) {
	while (samples.size() < sampleSize) {
		samples.push_back(data[rand() % data.size()]);
	}
}

void SampleSort::sortSamples(vector<int> &samples) {
	int sendBuffer[samples.size()];
	int *receiveBuffer = 0;
	size_t receiveSize = sampleSize * mpiSize;

	if (mpiRank == CUSTOM_MPI_ROOT) {
		receiveBuffer = new int[receiveSize];

		for (int i = 0; i < receiveSize; i++) {
			receiveBuffer[i] = 0;
		}

		cout << mpiRank << ": created receive buffer" << endl;
	}

	cout << mpiRank << ": receiveSize = " << receiveSize << endl;
	cout << mpiRank << ": sizeof(MPI_INT) = " << sizeof(MPI::INT) << endl;
	cout << mpiRank << ": MPI_INT = " << MPI::INT << endl;

	for (size_t i = 0; i < samples.size(); i++) {
		sendBuffer[i] = samples[i];
		cout << hex << samples[i] << ",";
	}

	cout << endl;

	if (mpiRank == CUSTOM_MPI_ROOT) {
		cout << mpiRank << ": copy local data" << endl;

		for (int i = 0; i < sampleSize; i++) {
			receiveBuffer[i] = sendBuffer[i];
		}

		for (int i = 1; i < mpiSize; i++) {
			cout << mpiRank << ": receiving from " << i << endl;
			COMM_WORLD.Recv(receiveBuffer + (i * sampleSize), sampleSize, MPI::INT, i, MPI::ANY_TAG);
		}
	} else {
		COMM_WORLD.Send(sendBuffer, sampleSize, MPI::INT, 0, 0);
	}

	//COMM_WORLD.Gather(sendBuffer, samples.size(), MPI::INT, receiveBuffer, receiveSize, MPI::INT, CUSTOM_MPI_ROOT);

	cout << mpiRank << ": After gather" << endl;
	COMM_WORLD.Barrier();

	splitter = new int[mpiSize - 1];

	if (mpiRank == CUSTOM_MPI_ROOT) {
		cout << "all samples = ";

		for (int i = 0; i < receiveSize; i++) {
			cout << hex << receiveBuffer[i] << ",";
		}

		cout << endl;

		std::sort(receiveBuffer, receiveBuffer + receiveSize);

		cout << "all samples sorted = ";

		for (int i = 0; i < receiveSize; i++) {
			cout << hex << receiveBuffer[i] << ",";
		}

		cout << endl;

		for (int i = 0; i < mpiSize - 1; i++) {
			splitter[i] = receiveBuffer[(i + 1) * sampleSize - 1];
		}

		cout << "splitter = ";

		for (int i = 0; i < mpiSize - 1; i++) {
			cout << hex << splitter[i] << ",";
		}

		cout << endl;

		delete receiveBuffer;
	}

	COMM_WORLD.Bcast(splitter, mpiSize - 1, MPI::INT, CUSTOM_MPI_ROOT);

	COMM_WORLD.Barrier();
	cout << mpiRank << ": Finished sorting samples" << endl;
}

void SampleSort::partitionData(vector<int> &data, vector<size_t> &positions) {
	// BINARY SEARCH FOR SPLITTER POSITIONS
	auto first = data.begin();

	positions.push_back(0);
	for (int i = 0; i < mpiSize - 1; i++) {
		first = lower_bound(first, data.end(), splitter[i]);
		positions.push_back(first - data.begin());
	}
}

void SampleSort::shareData(vector<int> &data, vector<size_t> &positions) {
	size_t *bucketSizes = new size_t[mpiSize];
	size_t *recBucketSizes = new size_t[mpiSize];

	for (int i = 0; i < mpiSize - 1; i++) {
		bucketSizes[i] = positions[i + 1] - positions[i];
	}

	bucketSizes[mpiSize - 1] = data.size() - positions[mpiSize - 1];

	COMM_WORLD.Alltoall(bucketSizes, 1, MPI::UNSIGNED_LONG, recBucketSizes, 1, MPI::UNSIGNED_LONG);

	size_t receiveSize = 0;

	for (int i = 0; i < mpiSize; i++) {
		receiveSize += recBucketSizes[i];
	}

	int *receivedData = new int[receiveSize];
	//size

	//COMM_WORLD.Alltoallv();
}

SampleSort::~SampleSort() {
	// TODO Auto-generated destructor stub
}

