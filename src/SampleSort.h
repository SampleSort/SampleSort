/*
 * SampleSort.h
 *
 *  Created on: 14.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESORT_H_
#define SAMPLESORT_H_

#include "mpi.h"

class SampleSort {
private:

public:
	SampleSort();
	void sort(int *data);
	virtual ~SampleSort();
};

#endif /* SAMPLESORT_H_ */
