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
#include "SortSamplesStrategy.h"
#include "SampleSortParams.h"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>
#include <chrono>


using namespace std;
using namespace MPI;



const int TEST_DATA_SIZE = 500000;
const int SEND_LOWEST_TAG = 0;
const int SEND_HIGHEST_TAG = 1;



void generateRandomData(vector<int> &data, int mpiSize) {
	default_random_engine randomGenerator(getSeed());
	uniform_int_distribution<int> randomDistribution(-data.size() * mpiSize, data.size() * mpiSize);

	for (int i = 0; i < data.capacity(); i++) {
		data[i] = randomDistribution(randomGenerator);
		//cout << data[i] << endl;
	}
}

void checkSorted(vector<int> &array, int mpiRank, int mpiSize) {
	cout << mpiRank << ": Enter checkSorting" << endl;

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

	COMM_WORLD.Gatherv(array.data(), array.size(), MPI::INT, allData.data(), gatheredSizes.data(), offsets.data(), MPI::INT, 0);

	if (mpiRank == 0) {
		for (int i = 1; i < allData.size(); i++) {
			if (allData[i - 1] > allData[i]) {
				cout << mpiRank << ": Sorting failed!" << endl;
				return;
			}
		}

		cout << mpiRank << ": Sorting complete!" << endl;
	}
}

int main(int argc, char *argv[]) {
	Init(argc, argv);
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	SampleSortParams params(mpiRank, mpiSize, 0, true, 5);
	GatherSortSamplesStrategy sortSamplesStrategy;
	SampleSort sorter(params, sortSamplesStrategy);
	vector<int> data(TEST_DATA_SIZE);
	generateRandomData(data, mpiSize);

	COMM_WORLD.Barrier();
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
	COMM_WORLD.Barrier();

	vector<int> result;
	sorter.sort(data, result);

	COMM_WORLD.Barrier();
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	cout << "Sorting took " << chrono::duration_cast<chrono::microseconds>(end - start).count() << "us" << endl;

	checkSorted(result, mpiRank, mpiSize);

	cout << mpiRank << ": Finalizing" << endl;
	Finalize();
}
