/*
 * SampleSizeStrategy.h
 *
 *  Created on: 24.03.2016
 *      Author: s_sschmi
 */

#ifndef SAMPLESIZESTRATEGY_H_
#define SAMPLESIZESTRATEGY_H_

class SampleSizeStrategy {
public:
	virtual int sampleSize(int globalDataSize) = 0;
	virtual ~SampleSizeStrategy() {}
};

#endif /* SAMPLESIZESTRATEGY_H_ */
