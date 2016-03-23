/*
 ============================================================================
 Name        : SampleSort
 Author      : Sebastian Schmidt, Lars Gottesbüren
 Version     :
 Copyright   : MIT License
 Description : Compute Pi in MPI C++
 ============================================================================
 */

#include "mpi.h"
#include "SampleSort.h"
#include "Random.h"
#include "GatherSortSamplesStrategy.h"
#include "RecursiveSortSamplesStrategy.h"
#include "SortSamplesStrategy.h"
#include "SampleSortParams.h"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace MPI;

const int TEST_DATA_SIZE = 500000;

void generateRandomData(vector<int> &data, int mpiSize) {
	default_random_engine randomGenerator(getSeed());
	uniform_int_distribution<int> randomDistribution(-data.size() * mpiSize,
			data.size() * mpiSize);

	for (int i = 0; i < data.capacity(); i++) {
		data[i] = randomDistribution(randomGenerator);
		//cout << data[i] << endl;
	}
}

bool checkSorted(vector<int> &array, int mpiRank, int mpiSize) {
	int size = array.size();
	vector<int> gatheredSizes;

	if (mpiRank == 0) {
		gatheredSizes.resize(mpiSize);
	}

	COMM_WORLD.Gather(&size, 1, MPI::INT, gatheredSizes.data(), 1, MPI::INT, 0);

	vector<int> allData;
	vector<int> offsets;

	if (mpiRank == 0) {
		offsets.resize(mpiSize);
		offsets[0] = 0;

		for (int i = 1; i < mpiSize; i++) {
			offsets[i] = gatheredSizes[i - 1] + offsets[i - 1];
		}

		allData.resize(offsets[mpiSize - 1] + gatheredSizes[mpiSize - 1]);
	}

	COMM_WORLD.Gatherv(array.data(), array.size(), MPI::INT, allData.data(),
			gatheredSizes.data(), offsets.data(), MPI::INT, 0);

	if (mpiRank == 0) {
		for (int i = 1; i < allData.size(); i++) {
			if (allData[i - 1] > allData[i]) {
				cout << "Sorting failed!" << endl;
				return false;
			}
		}

		cout << "Sorting correct!" << endl;
		return true;
	}

	return true;
}

unsigned long runTest(int recursiveThreshold) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	if (mpiRank == 0) {
		cout << "Running test with recursiveThreshold = " << recursiveThreshold
				<< endl;
	}

	SampleSortParams params(mpiRank, mpiSize, 0, true, -1);
	// GatherSortSamplesStrategy sortSamplesStrategy;
	RecursiveSortSamplesStrategy<int> sortSamplesStrategy(recursiveThreshold);
	SampleSort<int> sorter(params, sortSamplesStrategy);
	vector<int> data(TEST_DATA_SIZE);
	generateRandomData(data, mpiSize);

	COMM_WORLD.Barrier();
	unsigned long start =
			chrono::high_resolution_clock::now().time_since_epoch().count();

	vector<int> result;
	sorter.sort(data, result);

	unsigned long end =
			chrono::high_resolution_clock::now().time_since_epoch().count();
	COMM_WORLD.Barrier();

	unsigned long maxTime;
	unsigned long localTime = end - start;
	COMM_WORLD.Reduce(&localTime, &maxTime, 1, MPI::UNSIGNED_LONG, MPI::MAX, 0);

	double time = maxTime;
	time *= chrono::high_resolution_clock::period::type::num;
	time /= chrono::high_resolution_clock::period::type::den;

	if (mpiRank == 0) {
		//cout << "Sorting took " << (time * 1e6) << "us" << endl;
	}

	return time * 1e6;
}

void runTests(int runCount, int recursiveThreshold) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();
	vector<unsigned long> times;

	for (int i = 0; i < runCount; i++) {
		times.push_back(runTest(recursiveThreshold));
	}

	sort(times.begin(), times.end());

	if (mpiRank == 0) {
		cout << "Sorting time median = " << times[times.size() / 2] << "us"
				<< endl;
	}
}

int main(int argc, char *argv[]) {
	Init(argc, argv);
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	runTests(10, 50);

	Finalize();
}
