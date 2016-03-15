/*
 ============================================================================
 Name        : SampleSort
 Author      : Sebastian Schmidt, Lars Gottesbüren
 Version     :
 Copyright   : MIT License
 Description : Compute Pi in MPI C++
 ============================================================================
 */

#include "SampleSort.h"
#include "Random.h"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>


using namespace std;
using namespace MPI;



const int TEST_DATA_SIZE = 50000;
const int SEND_LOWEST_TAG = 0;
const int SEND_HIGHEST_TAG = 1;


int mpiSize;
int mpiRank;


void generateRandomData(vector<int> &data) {
	default_random_engine randomGenerator(getSeed());
	uniform_int_distribution<int> randomDistribution(-data.size() * mpiSize, data.size() * mpiSize);

	for (int i = 0; i < data.capacity(); i++) {
		data[i] = randomDistribution(randomGenerator);
		//cout << data[i] << endl;
	}
}

bool checkSorting(vector<int> &array) {
	DEBUG("Enter checkSorting")

	if (array.size() == 0) {
		DEBUG("Array is empty");
	}

	int fromLower = 0;
	int fromHigher = 0;
	int toLower = array[0];
	int toHigher = array[array.size() - 1];


	if (mpiRank < mpiSize - 1) {
		COMM_WORLD.Recv(&fromHigher, 1, MPI::INT, mpiRank + 1, SEND_LOWEST_TAG);
	}

	if (mpiRank > 0) {
		COMM_WORLD.Send(&toLower, 1, MPI::INT, mpiRank - 1, SEND_LOWEST_TAG);
	}

	if (mpiRank > 0) {
		COMM_WORLD.Recv(&fromLower, 1, MPI::INT, mpiRank - 1, SEND_HIGHEST_TAG);
	}

	if (mpiRank < mpiSize - 1) {
		COMM_WORLD.Send(&toHigher, 1, MPI::INT, mpiRank + 1, SEND_HIGHEST_TAG);
	}

	if ((mpiRank != 0 && fromLower > array[0]) || (mpiRank < mpiSize - 1 && fromHigher < array[mpiSize - 1])) {
		return false;
	}

	for (int i = 0; i < mpiSize - 1; i++) {
		if (array[i] > array[i + 1]) {
			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[]) {
	Init(argc, argv);
	mpiSize = COMM_WORLD.Get_size();
	mpiRank = COMM_WORLD.Get_rank();

	SampleSort sorter(mpiRank, mpiSize, true, 5);
	vector<int> data(TEST_DATA_SIZE);
	generateRandomData(data);

	vector<int> result = sorter.sort(data);

	if (checkSorting(result)) {
		cout << mpiRank << ": Sorting complete!" << endl;
	} else {
		cout << mpiRank << ": Sorting failed!" << endl;
	}

	DEBUG("Finalizing")
	Finalize();
}
