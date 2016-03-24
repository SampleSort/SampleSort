/*
 ============================================================================
 Name        : SampleSort
 Author      : Sebastian Schmidt, Lars Gottesbüren
 Version     :
 Copyright   : MIT License
 Description : Test the SampleSort algorithm smoothly and thoroughly.
 ============================================================================
 */

#include "mpi.h"
#include "SampleSort.h"
#include "Random.h"
#include "GatherSortSamplesStrategy.h"
#include "RecursiveSortSamplesStrategy.h"
#include "SortSamplesStrategy.h"
#include "SampleSortParams.h"
#include "Debug.h"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <thread>

using namespace std;
using namespace MPI;

const int BENCHMARK_DATA_SIZE = 500000;
const int TEST_DATA_SIZE = 50;

void generateRandomData(vector<int> &data, int minMax) {
	default_random_engine randomGenerator(getSeed());
	uniform_int_distribution<int> randomDistribution(-data.size() * minMax,
			data.size() * minMax);

	for (int i = 0; i < data.capacity(); i++) {
		data[i] = randomDistribution(randomGenerator);
		//cout << data[i] << endl;
	}
}

template<typename T>
bool checkSorted(vector<T> &array, int mpiRank, int mpiSize) {
	int size = array.size() * sizeof(T);
	vector<int> gatheredSizes;

	if (mpiRank == 0) {
		gatheredSizes.resize(mpiSize);
	}

	COMM_WORLD.Gather(&size, 1, MPI::INT, gatheredSizes.data(), 1, MPI::INT, 0);

	vector<T> allData;
	vector<int> offsets;

	if (mpiRank == 0) {
		offsets.resize(mpiSize);
		offsets[0] = 0;

		for (int i = 1; i < mpiSize; i++) {
			offsets[i] = gatheredSizes[i - 1] + offsets[i - 1];
		}

		allData.resize(
				(offsets[mpiSize - 1] + gatheredSizes[mpiSize - 1])
						/ sizeof(T));
	}

	COMM_WORLD.Gatherv(array.data(), array.size() * sizeof(T), MPI::BYTE,
			allData.data(), gatheredSizes.data(), offsets.data(), MPI::BYTE, 0);

	if (mpiRank == 0) {
		for (int i = 1; i < allData.size(); i++) {
			if (allData[i - 1] > allData[i]) {
				cout << "Sorting failed!" << endl;
				return false;
			}
		}

		cout << "Sorting correct!" << endl;
		return true;
	}

	return true;
}

unsigned long runTest(int recursiveThreshold) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	SampleSortParams params(mpiRank, mpiSize, 0, true, -1);
	// GatherSortSamplesStrategy sortSamplesStrategy;
	RecursiveSortSamplesStrategy<int> sortSamplesStrategy(recursiveThreshold);
	SampleSort<int> sorter(params, sortSamplesStrategy);
	vector<int> data(BENCHMARK_DATA_SIZE);
	generateRandomData(data, mpiSize);

	COMM_WORLD.Barrier();
	unsigned long start =
			chrono::high_resolution_clock::now().time_since_epoch().count();

	vector<int> result;
	sorter.sort(data, result);

	unsigned long end =
			chrono::high_resolution_clock::now().time_since_epoch().count();
	COMM_WORLD.Barrier();

	unsigned long maxTime;
	unsigned long localTime = end - start;
	COMM_WORLD.Reduce(&localTime, &maxTime, 1, MPI::UNSIGNED_LONG, MPI::MAX, 0);

	double time = maxTime;
	time *= chrono::high_resolution_clock::period::type::num;
	time /= chrono::high_resolution_clock::period::type::den;

	if (mpiRank == 0) {
		//cout << "Sorting took " << (time * 1e6) << "us" << endl;
	}

	return time * 1e6;
}

unsigned long runStdSort(int recursiveThreshold) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	if (mpiRank == 0) {
		vector<int> data(BENCHMARK_DATA_SIZE * mpiSize);
		generateRandomData(data, mpiSize);

		unsigned long start =
				chrono::high_resolution_clock::now().time_since_epoch().count();

		std::sort(data.begin(), data.end());

		unsigned long end =
				chrono::high_resolution_clock::now().time_since_epoch().count();

		unsigned long localTime = end - start;

		double time = localTime;
		time *= chrono::high_resolution_clock::period::type::num;
		time /= chrono::high_resolution_clock::period::type::den;

		if (mpiRank == 0) {
			//cout << "Sorting took " << (time * 1e6) << "us" << endl;
		}

		return time * 1e6;
	}

	return 0;
}

bool testAllEqualValue(int recursive_threshold, double value) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	vector<double> data(TEST_DATA_SIZE);
	for (vector<double>::iterator it = data.begin(); it != data.end(); it++) {
		*it = value;
	}

	SampleSortParams params(mpiRank, mpiSize, 0, true, -1);
	RecursiveSortSamplesStrategy<double> sortSamplesStrategy(
			recursive_threshold);
	SampleSort<double> sorter(params, sortSamplesStrategy);
	vector<double> results;
	sorter.sort(data, results);
	return checkSorted(results, mpiRank, mpiSize);
}

unsigned long runTests(const int warmUp, const int runCount,
		int recursiveThreshold, unsigned long test(int)) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();
	vector<unsigned long> times;

	for (int i = 0; i < warmUp; i++) {
		if (mpiRank == 0 && (i % 50 == 0 || i == warmUp - 1)) {
			cout << "Warm up " << (i + 1) << "/" << warmUp << endl;
		}

		test(recursiveThreshold);
	}

	for (int i = 0; i < runCount; i++) {
		if (mpiRank == 0 && (i % 50 == 0 || i == runCount - 1)) {
			cout << "Running test no " << (i + 1) << "/" << runCount
					<< " with recursiveThreshold = " << recursiveThreshold
					<< endl;
		}

		times.push_back(test(recursiveThreshold));
	}

	sort(times.begin(), times.end());
	unsigned long median = times[times.size() / 2];

	if (mpiRank == 0) {
		cout << "Sorting time median = " << median << "us" << endl;
	}

	return median;
}

int main(int argc, char *argv[]) {
	Init(argc, argv);
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	//testAllEqualValue(50, 456.345634756);
	unsigned concurrentThreadsSupported = thread::hardware_concurrency();
	cout << mpiRank << ": concurrentThreadsSupported = "
			<< concurrentThreadsSupported << endl;

	vector<int> thresholds;
	vector<unsigned long> ourMedians;
	unsigned long stdMedian;

	thresholds.push_back(10);
	thresholds.push_back(20);
	thresholds.push_back(40);
	thresholds.push_back(80);
	thresholds.push_back(160);
	thresholds.push_back(320);
	thresholds.push_back(1 << 30);

	COMM_WORLD.Barrier();

	cout << " ====== TESTING STD::SORT  ====== " << endl;
	stdMedian = runTests(5, 100, 50, runStdSort);

	cout << " ====== TESTING SAMPLESORT ====== " << endl;
	for (int i = 0; i < thresholds.size(); i++) {
		cout << " ====== ROUND " << (i + 1) << "/" << thresholds.size() << " ====== " << endl;
		ourMedians.push_back(runTests(10, 100, 50, runTest));
	}

	for (int i = 0; i < thresholds.size(); i++) {
		double speedUp = stdMedian / (double) ourMedians[i];
		double localEfficiency = speedUp / concurrentThreadsSupported;
		double globalEfficiency = speedUp / mpiSize;

		if (mpiRank == 0) {
			cout << "For threshold =         " << thresholds[i] << endl;
			cout << "  speedUp =             " << speedUp << endl;
			cout << "  efficiency (local) =  " << localEfficiency << endl;
			cout << "  efficiency (global) = " << globalEfficiency << endl;
		}
	}

	Finalize();
}

