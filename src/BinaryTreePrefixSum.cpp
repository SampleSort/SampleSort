/*
 * BinaryTreePrefixSum.cpp
 *
 *  Created on: 23.03.2016
 *      Author: s_sschmi
 */

#include "BinaryTreePrefixSum.h"

#define TAG 0

BinaryTreePrefixSum::BinaryTreePrefixSum() {
	// TODO Auto-generated constructor stub
	mpiSize = COMM_WORLD.Get_size();
	int remainder = mpiSize;
	int i;
	for (i = 0; remainder != 0; i++) {
		remainder/= 2;
	}
	root = 1 << (i - 1);

}

int BinaryTreePrefixSum::prefix_sum(int local_value) {
	const int mpiRank = COMM_WORLD.Get_rank();
	const int index = mpiRank + 1;

	int l = 0;
	int r = 0;

	if (has_left_child(index)) {
		COMM_WORLD.Recv(&l, 1, MPI::INT, left_child(index), TAG);
	}

	if (has_right_child(index)) {
		COMM_WORLD.Recv(&r, 1, MPI::INT, right_child(index), TAG);
	}

	int subtree_sum = l + r + local_value;
	int from_parent = 0;

	if (has_parent(index)) {
		COMM_WORLD.Send(&subtree_sum, 1, MPI::INT, parent(index), TAG);
		COMM_WORLD.Recv(&from_parent, 1, MPI::INT, parent(index), TAG);
	}

	int l_ = from_parent;

	if (has_left_child(index)) {
		COMM_WORLD.Send(&l_, 1, MPI::INT, left_child(index), TAG);
	}

	int my_prefix = from_parent + l;
	int r_ = my_prefix + local_value;

	if (has_right_child(index)) {
		COMM_WORLD.Send(&r_, 1, MPI::INT, right_child(index), TAG);
	}

	return my_prefix;
}

BinaryTreePrefixSum::~BinaryTreePrefixSum() {
	// TODO Auto-generated destructor stub
}
