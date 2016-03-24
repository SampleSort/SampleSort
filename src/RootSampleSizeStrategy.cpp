/*
 * RootSampleSizeStrategy.cpp
 *
 *  Created on: 24.03.2016
 *      Author: s_sschmi
 */

#include "RootSampleSizeStrategy.h"
#include <cmath>

using namespace std;

RootSampleSizeStrategy::RootSampleSizeStrategy(double rootExponent,
		double factor) :
		rootExponent(rootExponent), factor(factor) {
}

int RootSampleSizeStrategy::sampleSize(int globalDataSize) {
	return (int) (factor * pow(globalDataSize, 1 / rootExponent));
}

RootSampleSizeStrategy::~RootSampleSizeStrategy() {
}
