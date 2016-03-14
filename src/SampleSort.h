/*
 * SampleSort.h
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORT_H_
#define SAMPLESORT_H_

#include "~/include/mpi.h"

class SampleSort {
private:
	int size;
	int rank;

public:
	SampleSort();
	void openMPI(int argc, char *argv[]);
	void sort(int *data);
	void closeMPI();
	virtual ~SampleSort();
};

#endif /* SAMPLESORT_H_ */
