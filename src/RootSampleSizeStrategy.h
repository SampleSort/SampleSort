/*
 * RootSampleSizeStrategy.h
 *
 *  Created on: 24.03.2016
 *      Author: s_sschmi
 */

#ifndef ROOTSAMPLESIZESTRATEGY_H_
#define ROOTSAMPLESIZESTRATEGY_H_

#include "SampleSizeStrategy.h"

class RootSampleSizeStrategy: public SampleSizeStrategy {
private:
	double rootExponent;
	double factor;

public:
	RootSampleSizeStrategy(double rootExponent, double factor);
	int sampleSize(int globalDataSize);
	virtual ~RootSampleSizeStrategy();
};

#endif /* ROOTSAMPLESIZESTRATEGY_H_ */
