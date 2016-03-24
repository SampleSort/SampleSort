/*
 * LogSampleSizeStrategy.h
 *
 *  Created on: 24.03.2016
 *      Author: s_sschmi
 */

#ifndef LOGSAMPLESIZESTRATEGY_H_
#define LOGSAMPLESIZESTRATEGY_H_

#include "SampleSizeStrategy.h"

class LogSampleSizeStrategy: public SampleSizeStrategy {
private:
	const double factor;

public:
	LogSampleSizeStrategy(double factor);
	int sampleSize(int globalSampleSize);
	virtual ~LogSampleSizeStrategy() {}
};

#endif /* LOGSAMPLESIZESTRATEGY_H_ */
