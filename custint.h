/*

	simple interval struct with end-points double
	based on constant upward rounding, no
	rounding mode switch necessary during operations
	
	Marc Meidlinger
	May 2020
	
	NO WARRANTY OF CORRECT RESULTS (although I did my best to
	ensure correctness of the implementation)
	
	CAUTION: Every arithmetics routine cust^NNN_ZAB()
			 ASSUMES rounding mode is set to UPWARDS
			 
			 It is at the discrition of the user to
			 ensure this
	
	based on the article:

	Interval arithmetic with fixed rounding modw
	by S. Rump et al., 2016

*/

#ifndef _CUSTOMIZED_INTERVAL
#define _CUSTOMIZED_INTERVAL

#include "fenv.h"
#include "stdint.h"
#include "float.h"

// get the current rounding mode, store it and
// set to upward infinity
struct CustRoundingUpwards {
	int32_t mode;
	
	CustRoundingUpwards();
	virtual ~CustRoundingUpwards();
};


// interval arithmetics with double and constant
// rounding upwards (downwards simulated by sign change)

struct CustInterval {
	double left,right;
	
	CustInterval();
	CustInterval(const double);
	CustInterval(const double,const double);
};


// forward

void terminate_program(void);
int32_t custAdd_ZAB(CustInterval&,CustInterval&,CustInterval&);
int32_t custSub_ZAB(CustInterval&,CustInterval&,CustInterval&);
int32_t custMul_ZAB(CustInterval&,CustInterval&,CustInterval&);
int32_t custDiv_ZAB(CustInterval&,CustInterval&,CustInterval&);
inline double minimumDouble(const double,const double,const double,const double);
inline double maximumDouble(const double,const double,const double,const double);


#endif
