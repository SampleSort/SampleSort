/*
 * TestPrefix.cpp
 *
 *  Created on: 23.03.2016
 *      Author: s_sschmi
 */

#include "mpi.h"
#include "BinaryTreePrefixSum.h"

#define DEBUGV(x) cout << #x << " = " << x << endl;

using namespace MPI;
using namespace std;

int main(int argc, char *argv[]) {
	Init(argc, argv);
	const int mpiSize = COMM_WORLD.Get_size();

	if (COMM_WORLD.Get_rank() == 0) {
		BinaryTreePrefixSum btps;

		for (int i = 1; i <= mpiSize; i++) {
			cout << "Testing " << i << endl;

			int h = btps.height(i);
			DEBUGV(h);

			if (btps.has_left_child(i)) {
				int lc = btps.left_child(i);
				DEBUGV(lc);
			}

			if (btps.has_right_child(i)) {
				int rc = btps.right_child(i);
				DEBUGV(rc);
			}

			if (btps.has_parent(i)) {
				int p = btps.parent(i);
				DEBUGV(p);
			}
		}
	}

	Finalize();
	return 0;
}
