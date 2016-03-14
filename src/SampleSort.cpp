/*
 * SampleSort.cpp
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#include "SampleSort.h"
#include <algorithm>

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
	int *receiveBuffer;
	size_t receiveSize = sampleSize * mpiSize;

	if (mpiRank == CUSTOM_MPI_ROOT) {
		receiveBuffer = new int[receiveSize];
	}

	cout << mpiRank << ": receiveSize = " << receiveSize << endl;
	cout << mpiRank << ": sizeof(MPI_INT) = " << sizeof(MPI_INT) << endl;
	cout << mpiRank << ": MPI_INT = " << MPI_INT << endl;

	for (size_t i = 0; i < samples.size(); i++) {
		sendBuffer[i] = samples[i];
		cout << samples[i] << ",";
	}

	cout << endl;

	MPI_Gather(sendBuffer, samples.size(), MPI_INT, receiveBuffer, receiveSize, MPI_INT, CUSTOM_MPI_ROOT, COMM_WORLD);

	cout << mpiRank << ": After gather" << endl;
	MPI_Barrier(COMM_WORLD);

	if (mpiRank == CUSTOM_MPI_ROOT) {
		delete receiveBuffer;
	}

}

SampleSort::~SampleSort() {
	// TODO Auto-generated destructor stub
}

