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
#include "SampleSizeStrategy.h"
#include "LogSampleSizeStrategy.h"
#include "RootSampleSizeStrategy.h"
#include "Debug.h"
#include "KWayMerge.h"

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

		//cout << "Sorting correct!" << endl;
		return true;
	}

	return true;
}

unsigned long runTest(int recursiveThreshold, bool withPresort, int inputSize) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	LogSampleSizeStrategy sss(6);
	//RootSampleSizeStrategy sss(2, 1);
	SampleSortParams params(mpiRank, mpiSize, 0, withPresort, -1, sss);
	// GatherSortSamplesStrategy sortSamplesStrategy;
	RecursiveSortSamplesStrategy<int> sortSamplesStrategy(recursiveThreshold);
	SampleSort<int> sorter(params, sortSamplesStrategy);
	vector<int> data(inputSize / mpiSize);
	generateRandomData(data, mpiSize);

	COMM_WORLD.Barrier();
	unsigned long start =
			chrono::high_resolution_clock::now().time_since_epoch().count();

	vector<int> result;
	sorter.sort(data, result, inputSize / mpiSize);

	unsigned long end =
			chrono::high_resolution_clock::now().time_since_epoch().count();
	COMM_WORLD.Barrier();

	unsigned long maxTime;
	unsigned long localTime = end - start;
	COMM_WORLD.Reduce(&localTime, &maxTime, 1, MPI::UNSIGNED_LONG, MPI::MAX, 0);

	double time = maxTime;
	time *= chrono::high_resolution_clock::period::type::num;
	time /= chrono::high_resolution_clock::period::type::den;

	if (!checkSorted(result, mpiRank, mpiSize)) {
		throw runtime_error("Result not sorted!");
	}

	if (mpiRank == 0) {
		//cout << "Sorting took " << (time * 1e6) << "us" << endl;
	}

	return time * 1e6;
}

unsigned long runStdSort(int recursiveThreshold, bool withPresort,
		int inputSize) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	if (mpiRank == 0) {
		vector<int> data(inputSize);
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

	LogSampleSizeStrategy sss(10);
	SampleSortParams params(mpiRank, mpiSize, 0, true, -1, sss);
	RecursiveSortSamplesStrategy<double> sortSamplesStrategy(
			recursive_threshold);
	SampleSort<double> sorter(params, sortSamplesStrategy);
	vector<double> results;
	sorter.sort(data, results, TEST_DATA_SIZE * mpiSize);
	return checkSorted(results, mpiRank, mpiSize);
}

unsigned long runTests(const int warmUp, const int runCount,
		int recursiveThreshold, bool withPresort, int inputSize,
		unsigned long test(int, bool, int)) {
	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();
	vector<unsigned long> times;

	for (int i = 0; i < warmUp; i++) {
		if (mpiRank == 0 && (i % 50 == 0 || i == warmUp - 1)) {
			cout << "Warm up " << (i + 1) << "/" << warmUp << endl;
		}

		COMM_WORLD.Barrier();
		test(recursiveThreshold, withPresort, inputSize);
	}

	for (int i = 0; i < runCount; i++) {
		if (mpiRank == 0 && (i % 50 == 0 || i == runCount - 1)) {
			cout << "Running test no " << (i + 1) << "/" << runCount
					<< " with recursiveThreshold = " << recursiveThreshold
					<< " and with" << (withPresort ? "" : "out")
					<< " presort and with inputSize = " << inputSize << endl;
		}

		COMM_WORLD.Barrier();
		times.push_back(test(recursiveThreshold, withPresort, inputSize));
	}

	sort(times.begin(), times.end());
	unsigned long median = times[times.size() / 2];

	if (mpiRank == 0) {
		//cout << "Sorting time median = " << median << "us" << endl;
	}

	return median;
}

struct TestResult {
	int inputSize;
	bool withPresort;
	int threshold;
	unsigned long stdMedian;
	unsigned long ourMedian;
};

int main(int argc, char *argv[]) {
	Init(argc, argv);

	int mpiSize = COMM_WORLD.Get_size();
	int mpiRank = COMM_WORLD.Get_rank();

	//testAllEqualValue(50, 456.345634756);
	unsigned concurrentThreadsSupported = thread::hardware_concurrency();
	cout << mpiRank << ": concurrentThreadsSupported = "
			<< concurrentThreadsSupported << endl;

	vector<int> thresholds;
	vector<int> inputSizes;
	vector<TestResult> testResults;
	const int repetitions = 100;

	//thresholds.push_back(0);
	//thresholds.push_back(1);
	//thresholds.push_back(2);
	//thresholds.push_back(3);
	//thresholds.push_back(6);
	//thresholds.push_back(10);
	//thresholds.push_back(20);
	//thresholds.push_back(40);
	thresholds.push_back(80);
	//thresholds.push_back(160);
	//thresholds.push_back(320);
	thresholds.push_back(1 << 30);

	for (int i = 0; i < 23; i++) {
		inputSizes.push_back(1 << i);
	}

	COMM_WORLD.Barrier();

	for (int inputSize : inputSizes) {
		if (mpiRank == 0) {
			cout << " ====== TESTING STD::SORT  ====== " << endl;
		}

		struct TestResult testResult;
		testResult.inputSize = inputSize;
		testResult.stdMedian = runTests(10, repetitions, -1, false, inputSize,
				runStdSort);

		if (mpiRank == 0) {
			cout << " ====== TESTING SAMPLESORT ====== " << endl;
		}

		for (int i = 0; i < thresholds.size(); i++) {
			if (mpiRank == 0) {
				cout << " ====== ROUND " << (i + 1) << "/" << thresholds.size()
						<< " ====== " << endl;
			}

			testResult.threshold = thresholds[i];

			testResult.withPresort = true;
			testResult.ourMedian = runTests(10, repetitions, thresholds[i],
					true, inputSize, runTest);
			testResults.push_back(testResult);

			testResult.withPresort = false;
			testResult.ourMedian = runTests(10, repetitions, thresholds[i],
					false, inputSize, runTest);
			testResults.push_back(testResult);
		}
	}

	if (mpiRank == 0) {
		int index = 0;

		for (struct TestResult testResult : testResults) {
			double speedUp = testResult.stdMedian
					/ (double) testResult.ourMedian;
			double localEfficiency = speedUp / concurrentThreadsSupported;
			double globalEfficiency = speedUp / mpiSize;

			cout << "For threshold =         " << testResult.threshold << endl;
			cout << "  MPI size =            " << mpiSize << endl;
			cout << "  local size =          " << concurrentThreadsSupported
					<< endl;
			cout << "  input size =          " << testResult.inputSize << endl;
			cout << "  repetitions =         " << repetitions << endl;
			cout << "  presort =             " << testResult.withPresort
					<< endl;
			cout << "  median runtime =      " << testResult.ourMedian << "us"
					<< endl;
			cout << "  speedUp =             " << speedUp << endl;
			cout << "  efficiency (local) =  " << localEfficiency << endl;
			cout << "  efficiency (global) = " << globalEfficiency << endl;
		}

		cout << "<><><><><><><><><><><><><><><><><><><><>" << endl;
		cout << "<><><><><><> PGFPLOT OUTPUT <><><><><><>" << endl;
		cout << "<><><><><><><><><><><><><><><><><><><><>" << endl;

		cout << " ==== STD::SORT  ====" << endl;
		cout << "Runtime:" << endl;
		for (int i = 0; i < testResults.size(); i += thresholds.size() * 2) {
			cout << "(" << testResults[i].inputSize << ", "
					<< testResults[i].stdMedian << ") ";
		}
		cout << endl;

		cout << " ==== SAMPLESORT ====" << endl;
		for (int i = 0; i < thresholds.size() * 2; i++) {
			cout << "Runtime (threshold = " << testResults[i].threshold
					<< ", withPresort = " << testResults[i].withPresort << "):"
					<< endl;

			for (int j = i; j < testResults.size();
					j += thresholds.size() * 2) {
				cout << "(" << testResults[j].inputSize << ", "
						<< testResults[j].ourMedian << ") ";
			}
			cout << endl;
		}

		for (int i = 0; i < thresholds.size() * 2; i++) {
			cout << "Speedup (threshold = " << testResults[i].threshold
					<< ", withPresort = " << testResults[i].withPresort << "):"
					<< endl;

			for (int j = i; j < testResults.size();
					j += thresholds.size() * 2) {
				cout << "(" << testResults[j].inputSize << ", "
						<< (testResults[j].stdMedian
								/ (double) testResults[j].ourMedian) << ") ";
			}
			cout << endl;
		}
	}

	Finalize();
}
