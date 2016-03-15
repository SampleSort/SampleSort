/*
 * Random.cpp
 *
 *  Created on: 15.03.2016
 *      Author: sibbo
 */

#include <chrono>

using namespace std::chrono;

unsigned getSeed() {
	return high_resolution_clock::now().time_since_epoch().count();
}
