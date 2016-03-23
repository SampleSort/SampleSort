/*
 * BinaryTreePrefixSum.h
 *
 *  Created on: 23.03.2016
 *      Author: s_sschmi
 */

#ifndef BINARYTREEPREFIXSUM_H_
#define BINARYTREEPREFIXSUM_H_

#include <vector>
#include "mpi.h"

using namespace std;
using namespace MPI;

class BinaryTreePrefixSum {
private:
	int mpiSize;
	int root;
	inline int height(int index) {
			int remainder = index;
			int i;
			for (i = 0; remainder % 2 == 0; i++) {
				remainder /= 2;
			}
			return i;
		};
	inline bool has_left_child(int index) {
		return height( index ) > 0;
	}
	inline bool has_right_child(int index) {
		return height(index) > 0 && index < mpiSize;
	}
	inline bool has_parent(int index) {
		return index != root;
	}
	inline int left_child(int index) {
		return index + (1 << ( height(index) - 1 ));
	}
	inline int right_child(int index);
	int parent(int index);

public:
	BinaryTreePrefixSum();
	virtual ~BinaryTreePrefixSum();
	int prefix_sum(int local_value);
};


#endif /* BINARYTREEPREFIXSUM_H_ */
