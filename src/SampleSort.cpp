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
}

void SampleSort::sort(vector<int> &data) {
	if (presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<int> samples;
	drawSamples(data, samples);
	sortSamples(samples);
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

	if (mpiRank == CUSTOM_MPI_ROOT) {
		for (int i = 0; i < receiveSize; i++) {
			cout << hex << receiveBuffer[i] << ",";
		}

		cout << endl;

		delete receiveBuffer;
	}

	COMM_WORLD.Barrier();
	cout << mpiRank << ": Finished sorting samples" << endl;
}

SampleSort::~SampleSort() {
	// TODO Auto-generated destructor stub
}

