#ifndef DEBUG_H_
#define DEBUG_H_

#define DO_DEBUG

#ifdef DO_DEBUG
	#define DEBUG(x) cout << p.mpiRank << ": " << x << endl;
	#define DEBUGV(x) cout << p.mpiRank << ": " << #x << " = " << x << endl;
#else
	#define DEBUG(x) ;
	#define DEBUGV(x) ;
#endif /* DO_DEBUG */
#endif /* DEBUG_H_ */
