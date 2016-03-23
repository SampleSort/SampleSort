/*
 * BinaryTreePrefixSum.cpp
 *
 *  Created on: 23.03.2016
 *      Author: s_sschmi
 */

#include "BinaryTreePrefixSum.h"

#define TAG 4
#define DEBUG(x) cout << index << ": " << x << endl;

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

	DEBUG("Starting prefix sum");

	int l = 0;
	int r = 0;
	int left_child_rank = left_child(index) - 1;
	int right_child_rank = right_child(index) - 1;
	int parent_rank = parent(index) - 1;

	if (index == 8) {
		cout << left_child_rank << "  " << right_child_rank << "  " << parent_rank << endl;
	}

	if (has_left_child(index)) {
		DEBUG("Receiving from left child");
		COMM_WORLD.Recv(&l, 1, MPI::INT, left_child_rank, TAG);
	}

	if (has_right_child(index)) {
		DEBUG("Receiving from right child");
		COMM_WORLD.Recv(&r, 1, MPI::INT, right_child_rank, TAG);
	}

	int subtree_sum = l + r + local_value;
	int from_parent = 0;

	if (has_parent(index)) {
		DEBUG("Sending to parent");
		COMM_WORLD.Send(&subtree_sum, 1, MPI::INT, parent_rank, TAG);

		DEBUG("Receiving from parent");
		COMM_WORLD.Recv(&from_parent, 1, MPI::INT, parent_rank, TAG);
	}

	int to_left = from_parent;
	int my_prefix = from_parent + l;
	int to_right = my_prefix + local_value;

	if (has_left_child(index)) {
		DEBUG("Sending to left child");
		COMM_WORLD.Send(&to_left, 1, MPI::INT, left_child_rank, TAG);
	}

	if (has_right_child(index)) {
		DEBUG("Sending to right child");
		COMM_WORLD.Send(&to_right, 1, MPI::INT, right_child_rank, TAG);
	}

	return my_prefix;
}

BinaryTreePrefixSum::~BinaryTreePrefixSum() {
	// TODO Auto-generated destructor stub
}
