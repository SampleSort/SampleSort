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

#include "SampleSort.h"

using namespace std;

const int TEST_DATA_SIZE = 50000;
int size;
int rank;


void generateRandomData(int *array, int size) {
	for (int i = 0; i < size; i++) {
		array[i] = rand();
	}
}

bool checkSorting(int *array, int size) {
	for (int i = 0; i < size - 1; i++) {
		if (array[i] > array[i + 1]) {
			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[]) {
	MPI::Init(argc, argv);
	size = MPI::COMM_WORLD.Get_size();
	rank = MPI::COMM_WORLD.Get_rank();

	SampleSort sorter;
	int data[TEST_DATA_SIZE];
	generateRandomData(data, TEST_DATA_SIZE);

	sorter.sort(data);

	if (checkSorting(data, TEST_DATA_SIZE)) {
		cout << "Sorting complete!" << endl;
	} else {
		cout << "Sorting failed!" << endl;
	}

	MPI::Finalize();
}
