#ifndef DEBUG_H_
#define DEBUG_H_

//#define DO_DEBUG

#ifdef DO_DEBUG
	#define DEBUG(x) cout << COMM_WORLD.Get_rank() << ": " << x << endl;
	#define DEBUGV(x) cout << COMM_WORLD.Get_rank() << ": " << #x << " = " << x << endl;
#else
	#define DEBUG(x) ;
	#define DEBUGV(x) ;
#endif /* DO_DEBUG */

//#define DO_INFO

#ifdef DO_INFO
	#define INFO(x) cout << COMM_WORLD.Get_rank() << ": " << x << endl;
	#define INFOR(x) if (COMM_WORLD.Get_rank() == 0) {cout << #x << " = " << x << endl;}
#else
	#define INFO(x) ;
	#define INFOR(x) ;
#endif /* DO_INFO */
#endif /* DEBUG_H_ */
