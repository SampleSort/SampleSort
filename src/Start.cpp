/*
 ============================================================================
 Name        : SampleSort
 Author      : Sebastian Schmidt, Lars Gottesbüren
 Version     :
 Copyright   : MIT License
 Description : Compute Pi in MPI C++
 ============================================================================
 */

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "SampleSort.h"

using namespace std;
using namespace MPI;



const int TEST_DATA_SIZE = 50000;
const int SEND_LOWEST_TAG = 0;
const int SEND_HIGHEST_TAG = 1;


int mpiSize;
int mpiRank;


void generateRandomData(vector<int> &data) {
	for (int i = 0; i < data.capacity(); i++) {
		data[i] = rand();
	}
}

bool checkSorting(vector<int> &array) {
	MPI_Request sendLowest;
	MPI_Request sendHighest;
	MPI_Status recvFromLower;
	MPI_Status recvFromHigher;

	if (mpiRank > 0) {
		MPI_Isend(&array[0], 1, MPI_INT, mpiRank - 1, SEND_LOWEST_TAG, COMM_WORLD, &sendLowest);
	}

	if (mpiRank < mpiSize - 1) {
		MPI_Isend(&array[mpiSize - 1], 1, MPI_INT, mpiRank + 1, SEND_HIGHEST_TAG, COMM_WORLD, &sendHighest);
	}

	int fromLower = 0;
	int fromHigher = 0;

	if (mpiRank > 0) {
		MPI_Recv(&fromLower, 1, MPI_INT, mpiRank - 1, SEND_HIGHEST_TAG, COMM_WORLD, &recvFromLower);
	}

	if (mpiRank < mpiSize - 1) {
		MPI_Recv(&fromHigher, 1, MPI_INT, mpiRank + 1, SEND_LOWEST_TAG, COMM_WORLD, &recvFromHigher);
	}

	if (fromLower > array[0] || fromHigher < array[mpiSize - 1]) {
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
	// TODO srand(5);

	Init(argc, argv);
	mpiSize = COMM_WORLD.Get_size();
	mpiRank = COMM_WORLD.Get_rank();

	SampleSort sorter(mpiRank, mpiSize, true, 5);
	vector<int> data(TEST_DATA_SIZE);
	generateRandomData(data);

	sorter.sort(data);

	if (checkSorting(data)) {
		cout << "Sorting complete!" << endl;
	} else {
		cout << "Sorting failed!" << endl;
	}

	Finalize();
}
