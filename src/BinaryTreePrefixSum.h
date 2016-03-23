/*
 * BinaryTreePrefixSum.h
 *
 *  Created on: 23.03.2016
 *      Author: s_sschmi
 */

#ifndef BINARYTREEPREFIXSUM_H_
#define BINARYTREEPREFIXSUM_H_

#include <vector>
#include <exception>
#include "mpi.h"

#include <iostream>

using namespace std;
using namespace MPI;

class BinaryTreePrefixSum {
public: // TODO private
	int mpiSize;
	int root;

	inline void checkIndexRange(int index) {
		if (index < 1 || index > mpiSize) {
			throw runtime_error("Wrong index");
		}
	}

	inline int height_unchecked(int index) {
		int remainder = index;
		int i;

		for (i = 0; remainder % 2 == 0; i++) {
			remainder /= 2;
		}

		return i;
	}

	inline int height(int index) {
		checkIndexRange(index);
		return height_unchecked(index);
	}

	inline bool has_left_child(int index) {
		checkIndexRange(index);
		return height(index) > 0;
	}

	inline bool has_right_child(int index) {
		checkIndexRange(index);
		return height(index) > 0 && index < mpiSize;
	}

	inline bool has_parent(int index) {
		checkIndexRange(index);
		return index != root;
	}

	inline int left_child(int index) {
		checkIndexRange(index);
		return left_child_unchecked(index);
	}

	inline int left_child_unchecked(int index) {
		return index - (1 << (height_unchecked(index) - 1));
	}

	inline int right_child(int index) {
		checkIndexRange(index);
		int rc = index + (1 << (height(index) - 1));

		while (rc > mpiSize) {
			rc = left_child_unchecked(rc);
		}

		return rc;
	}

	inline int parent(const int index) {
		checkIndexRange(index);
		int parent = index;
		int h = height(index);

		do {
			parent &= ~(1 << h);
			parent |= 1 << (h + 1);
			h++;
		} while (parent > mpiSize);

		return parent;
	}

public:
	BinaryTreePrefixSum();
	virtual ~BinaryTreePrefixSum();
	int prefix_sum(int local_value);
};

#endif /* BINARYTREEPREFIXSUM_H_ */
