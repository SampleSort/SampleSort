/*
 * LogSampleSizeStrategy.cpp
 *
 *  Created on: 24.03.2016
 *      Author: s_sschmi
 */

#include "LogSampleSizeStrategy.h"
#include <cmath>

LogSampleSizeStrategy::LogSampleSizeStrategy(double factor) : factor(factor) {
}

int LogSampleSizeStrategy::sampleSize(int globalDataSize) {
	return (int) (log2(globalDataSize) * factor);
}
