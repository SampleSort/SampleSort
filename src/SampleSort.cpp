/*
 * SampleSort.cpp
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#include "SampleSort.h"

SampleSort::SampleSort() {
	size = -1;
	rank = -1;
}

void SampleSort::openMPI(int argc, char *argv[]) {
	MPI::Init(argc, argv);
	size = MPI::COMM_WORLD.Get_size();
	rank = MPI::COMM_WORLD.Get_rank();
}

void SampleSort::sort(int *data) {

}

void SampleSort::closeMPI() {
	MPI::Finalize();
}

SampleSort::~SampleSort() {
	// TODO Auto-generated destructor stub
}

