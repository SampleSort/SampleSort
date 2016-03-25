/*
 * SampleSort.cpp
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#include "mpi.h"
#include "SampleSort.h"
#include "Random.h"
#include "KWayMerge.h"

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
void SampleSort<T>::sort(vector<T> &data, vector<T> &sortedData,
		int globalDataSize) {
	if (p.presortLocalData) {
		std::sort(data.begin(), data.end());
	}

	vector<T> samples;
	vector<int> positions;
	vector<T> splitter(p.mpiSize - 1);
	vector<int> receivePositions;

	p.sampleSize = min(p.sampleSizeStrategy.sampleSize(globalDataSize),
			(int) data.size() / 2);
	p.sampleSize = max(p.sampleSize, 1);
	INFOR(p.sampleSize);

	//DEBUGV(p.sampleSize);
	drawSamples(data, samples);
	//DEBUG("Drew samples");
	sortSamplesStrategy.sortSamples(samples, splitter, p);
	//DEBUG("Sorted samples");
	partitionData(data, splitter, positions);
	//DEBUG("Partitioned data");
	shareData(data, positions, sortedData, receivePositions);
	//DEBUG("Shared data");
	sortData(sortedData, receivePositions);
	//DEBUG("Sorted data");
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
	if (p.presortLocalData) {
// BINARY SEARCH FOR SPLITTER POSITIONS
		typename vector<T>::iterator first = data.begin();

		positions.push_back(0);
		for (int i = 0; i < p.mpiSize - 1; i++) {
			first = lower_bound(first, data.end(), splitter[i]);
			positions.push_back(first - data.begin());
		}
	} else {
		vector<vector<T>> buckets(splitter.size() + 1);

		for (int i = 0; i < buckets.size(); i++) {
			buckets[i].reserve(data.size() / this->p.mpiSize * 2);
		}

		for (int i = 0; i < data.size(); i++) {
			int bucketPosition = lower_bound(splitter.begin(), splitter.end(),
					data[i]) - splitter.begin();
			buckets[bucketPosition].push_back(data[i]);
		}

		int currentPosition = 0;
		for (int i = 0; i < buckets.size(); i++) {
			positions.push_back(currentPosition);

			for (int j = 0; j < buckets[i].size(); j++) {
				data[currentPosition++] = buckets[i][j];
			}
		}
	}
}

template<typename T>
void SampleSort<T>::shareData(vector<T> &data, vector<int> &positions,
		vector<T> &receivedData, vector<int> &receivePositions) {
	int *bucketSizes = new int[p.mpiSize];
	int *recBucketSizes = new int[p.mpiSize];

	cout << p.mpiRank << ": sendData = " << hex;
	for (int i = 0; i < data.size(); i++) {
		cout << data[i] << " ";
	}
	cout << dec << endl;

	cout << p.mpiRank << ": positions = ";
	// Adjust positions for data type size
	for (int i = 0; i < positions.size(); i++) {
		positions[i] *= sizeof(T);
		cout << positions[i] << " ";
	}
	cout << endl;

	// Calculate our bucket sizes
	for (int i = 0; i < p.mpiSize - 1; i++) {
		bucketSizes[i] = positions[i + 1] - positions[i];
	}

	bucketSizes[p.mpiSize - 1] = sizeof(T) * data.size() - positions[p.mpiSize - 1];

	cout << p.mpiRank << ": bucketSizes = ";
	for (int i = 0; i < p.mpiSize; i++) {
		cout << bucketSizes[i] << " ";
	}
	cout << endl;

	// Exchange bucket sizes. Receive how many elements we receive from every other PE.
	COMM_WORLD.Alltoall(bucketSizes, 1, MPI::INT, recBucketSizes, 1, MPI::INT);

	cout << p.mpiRank << ": recBucketSizes = ";
	for (int i = 0; i < p.mpiSize; i++) {
		cout << recBucketSizes[i] << " ";
	}
	cout << endl;

	int receiveSize = 0;

	// Count how many elements we receive from all other PEs added up.
	for (int i = 0; i < p.mpiSize; i++) {
		receiveSize += recBucketSizes[i];
	}

	receivedData.resize(receiveSize / sizeof(T));

	if (receiveSize / sizeof(T) == 0) {
		receivedData.reserve(1);
		DEBUG("Receiving nothing!");
	}

	// Fill receivedData with deterministic error values
	for (int i = 0; i < receivedData.size(); i++) {
		receivedData[i] = 0;
	}

	receivePositions.resize(p.mpiSize);

	// Calculate the offsets in the received data buffer for every PE.
	receivePositions[0] = 0;
	for (int i = 1; i < p.mpiSize; i++) {
		receivePositions[i] = recBucketSizes[i - 1] + receivePositions[i - 1];
	}

	cout << p.mpiRank << ": receivePositions = ";
	for (int i = 0; i < receivePositions.size(); i++) {
		cout << receivePositions[i] << " ";
	}
	cout << endl;

	// Has calculated how much data from which node is received at which position in our receiver array.
	cout << p.mpiRank << ": sendData = " << hex;
	for (int i = 0; i < data.size() * sizeof(T); i++) {
		cout << setw(2) << setfill('0') << (unsigned int) ((unsigned char*) data.data())[i] << " ";
	}
	cout << dec << endl;

	COMM_WORLD.Alltoallv((char*) data.data(), bucketSizes, positions.data(), MPI::CHAR,
			(char*) receivedData.data(), recBucketSizes, receivePositions.data(),
			MPI::CHAR);

	cout << p.mpiRank << ": receivedData = " << hex;
	for (int i = 0; i < receivedData.size() * sizeof(T); i++) {
		cout << setw(2) << setfill('0') << (unsigned int) ((unsigned char*) receivedData.data())[i] << " ";
	}
	cout << dec << endl;

	delete bucketSizes;
	delete recBucketSizes;

	COMM_WORLD.Barrier();
	exit(0);
}

template<typename T>
void SampleSort<T>::sortData(vector<T> &receivedData,
		vector<int> &receivePositions) {
	cout << p.mpiRank << ": receivedData = ";
	for (int i = 0; i < receivedData.size(); i++) {
		cout << receivedData[i] << " ";
	}
	cout << endl;

	cout << p.mpiRank << ": receivePositions = ";
	for (int i = 0; i < receivePositions.size(); i++) {
		cout << receivePositions[i] / sizeof(T) << " ";

		if (receivePositions[i] % sizeof(T) != 0) {
			throw runtime_error("Receive position wrong");
		}
	}
	cout << endl;

	if (p.presortLocalData) {
		vector<vector<T>> sortedLists;
		sortedLists.reserve(receivePositions.size());
		receivePositions.push_back(receivedData.size() * sizeof(T));

		for (int i = 0; i < receivePositions.size() - 1; i++) {
			vector<T> sortedList;
			const int length = (receivePositions[i + 1] - receivePositions[i])
					/ sizeof(T);

			cout << "length = " << length << endl;

			sortedList.reserve(length);

			for (int j = 0; j < length; j++) {
				T before = 0;
				if (j > 0) {
					before = sortedList.back();
				}

				sortedList.push_back(
						receivedData[j + receivePositions[i] / sizeof(T)]);
				T after = sortedList.back();

				if (j > 0 && before > after) {
					throw runtime_error("List is not sorted");
				}
			}

			sortedLists.push_back(sortedList);
		}

		DEBUG("Doing merge");
		KWayMerge<T> merger;
		merger.kway_merge(sortedLists, receivedData);
		DEBUG("Did merge");
	} else {
		std::sort(receivedData.begin(), receivedData.end());
	}
}

template<typename T>
SampleSort<T>::~SampleSort() {
// TODO Auto-generated destructor stub
}
