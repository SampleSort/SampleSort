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
	MPI_Status recvFromLower;
	MPI_Status recvFromHigher;

	int fromLower = 0;
	int fromHigher = 0;
	int toLower = array[0];
	int toHigher = array[array.size() - 1];


	if (mpiRank < mpiSize - 1) {
		MPI_Recv(&fromHigher, 1, MPI_INT, mpiRank + 1, SEND_LOWEST_TAG, COMM_WORLD, &recvFromHigher);
	}

	if (mpiRank > 0) {
		MPI_Send(&toLower, 1, MPI_INT, mpiRank - 1, SEND_LOWEST_TAG, COMM_WORLD);
	}

	if (mpiRank > 0) {
		MPI_Recv(&fromLower, 1, MPI_INT, mpiRank - 1, SEND_HIGHEST_TAG, COMM_WORLD, &recvFromLower);
	}

	if (mpiRank < mpiSize - 1) {
		MPI_Send(&toHigher, 1, MPI_INT, mpiRank + 1, SEND_HIGHEST_TAG, COMM_WORLD);
	}

	cout << mpiRank << ": Received sorting check data" << endl;
	MPI_Barrier(COMM_WORLD);

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
	Init(argc, argv);
	mpiSize = COMM_WORLD.Get_size();
	mpiRank = COMM_WORLD.Get_rank();

	srand((mpiRank << 16) + mpiRank);

	SampleSort sorter(mpiRank, mpiSize, true, 5);
	vector<int> data(TEST_DATA_SIZE);
	generateRandomData(data);

	sorter.sort(data);

	cout << mpiRank << ": Finished sorting" << endl;
	MPI_Barrier(COMM_WORLD);

//	if (checkSorting(data)) {
//		cout << mpiRank << ": Sorting complete!" << endl;
//	} else {
//		cout << mpiRank << ": Sorting failed!" << endl;
//	}


	MPI_Barrier(COMM_WORLD);
	Finalize();
}