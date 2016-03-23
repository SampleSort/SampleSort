/*
 * BinaryTreePrefixSum.cpp
 *
 *  Created on: 23.03.2016
 *      Author: s_sschmi
 */

#include "BinaryTreePrefixSum.h"

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

BinaryTreePrefixSum::~BinaryTreePrefixSum() {
	// TODO Auto-generated destructor stub
}


int BinaryTreePrefixSum::left_child(int index)
int BinaryTreePrefixSum::right_child(int index)
int BinaryTreePrefixSum::parent(int index)
