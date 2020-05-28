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

#include "custint.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "fenv.h"


// general function

void terminate_program(void) {
	fprintf(stderr,"Program terminates.\n");
	exit(99);
}

double minimumDouble(
	const double a,const double b,
	const double c,const double d
) {
	double m=a;
	if (b < m) m=b;
	if (c < m) m=c;
	if (d < m) m=d;
	
	return m;
}
	
double maximumDouble(
	const double a,const double b,
	const double c,const double d
) {
	double m=a;
	if (b > m) m=b;
	if (c > m) m=c;
	if (d > m) m=d;
	
	return m;
}


// struct CustRoundingUpwards 
// the constructor retrieves the current rounding mode and
// saves it, then sets to upward infinity
// the destructor resets to the old rounding mode

CustRoundingUpwards::CustRoundingUpwards() {
	mode=fegetround();
	if (mode<0) {
		fprintf(stderr,"CustRoundingUpwards::construct. Cannot get current rounding mode.\n");
		terminate_program();
	}
	if (fesetround(FE_UPWARD) != 0) {
		fprintf(stderr,"CustRoundingUpwards::constructor. Cannot set rounding mode to upwards.\n");
		terminate_program();
	}
}

CustRoundingUpwards::~CustRoundingUpwards() {
	if (fesetround(mode) != 0) {
		fprintf(stderr,"CustRoundingUpwards::destructor. Cannot reset rounding mode to %i.\n",mode);
		terminate_program();
	}
}


// CustInterval

CustInterval::CustInterval() {
	// do nothing for speed reasons
}

CustInterval::CustInterval(const double a) {
	left=right=a;
	int32_t fpa=fpclassify(a);
	if (
		(fpa != FP_NORMAL) &&
		(fpa != FP_ZERO)
	) {
		fprintf(stderr,"CustInterval::constructor. Error, only normal floating-points (and 0) are allowed.\n");
		terminate_program();
	}
}

CustInterval::CustInterval(const double a,const double b) {
	left=a;
	right=b;
	int32_t fpa=fpclassify(a);
	int32_t fpb=fpclassify(b);
	if (
		(
			(fpa != FP_NORMAL) &&
			(fpa != FP_ZERO)
		) ||
		(
			(fpb != FP_NORMAL) &&
			(fpb != FP_ZERO)
		)
	) {
		fprintf(stderr,"CustInterval::constructor. Error, only normal floating-points (and 0) are allowed.\n");
		terminate_program();
	}
	
	if (a > b) {
		fprintf(stderr,"CustInterval::constructor. Not an interval. End points in wrong order.\n");
		terminate_program();
	}
}


// arithmetics

// all routines assume that the given parameters
// are normal or zero (i.e. no Inf, NaN or subnormals)
// (except division, that checks for the dividend
// not containing zero)
// resulting intervals are always normals or zero at
// end points (returning value 0).
// If not possible, the return value is -1
// and should be handled by the calling function appropriately

// a subnormal resulting interval end point is
// moved towards a circumferencing next normal (or zero)
#define IAVALIDATE_LEFT(VAR) \
{\
	switch (fpclassify(VAR)) {\
		case FP_INFINITE:\
		case FP_NAN: return -1; /* error */\
		case FP_SUBNORMAL: {\
			if (VAR > 0.0) {\
				/* left end, positive subnormal */\
				/* move to 0 */\
				VAR=0.0;\
			} else if (VAR < 0.0) {\
				/* left end, negative subnormal */\
				/* move to -FMIN */\
				VAR=-DBL_MIN;\
			}\
		}\
		default: {\
			break;\
		}\
	}\
}

#define IAVALIDATE_RIGHT(VAR) \
{\
	switch (fpclassify(VAR)) {\
		case FP_INFINITE:\
		case FP_NAN: return -1; /* error */\
		case FP_SUBNORMAL: {\
			if (VAR > 0.0) {\
				/* right end, positive subnormal */\
				/* move to smallest double */\
				VAR=DBL_MIN;\
			} else if (VAR < 0.0) {\
				/* right end, negative subnormal */\
				/* move to 0 */\
				VAR=0.0;\
			}\
		}\
		default: {\
			break;\
		}\
	}\
}

int32_t custAdd_ZAB(
	CustInterval& res,
	CustInterval& A,
	CustInterval& B
) {
	// /////////////////////////////////////////
	// rounding mode is assumed UPWARD
	// /////////////////////////////////////////

	// it is assumed that res is not the same object as A or B

	// left end: simulates downwards rounding by
	// consecutive negation (see articel by S.Rump)
	double a1=A.left; a1=-a1; // no rounding occurs
	double b1=B.left; b1=-b1; // no rounding occurs
	
	// ROUNDING upwards
	res.left=a1+b1; 
	
	res.left=-res.left; // no rounding
	// now the result is DOWNWARD-rounded A.left+B.left
	
	// right end-point: uses upward rounding
	res.right=A.right+B.right;
	
	// if result is subnormal => enlarge interval or
	// return error value -1 if infinity or NaN
	IAVALIDATE_LEFT(res.left);
	IAVALIDATE_RIGHT(res.right);
	
	// everything worked, valid interval in variable res
	return 0;
}
	
int32_t custSub_ZAB(
	CustInterval& res,
	CustInterval& A,
	CustInterval& B
) {
	// /////////////////////////////////////////
	// rounding mode is assumed UPWARD
	// /////////////////////////////////////////

	// it is assumed that res is not the same object as A or B

	// [a0..a1] - [b0..b1] = [a0..a1] + [-b1..-b0]
	
	CustInterval mB;
	mB.left=-B.right; // no rounding
	mB.right=-B.left; // no rounding
	
	return custAdd_ZAB(res,A,mB);
	// already corrected for subnormals
}

int32_t custMul_ZAB(
	CustInterval& res,
	CustInterval& A,
	CustInterval& B
) {
	// /////////////////////////////////////////
	// rounding mode is assumed UPWARD
	// /////////////////////////////////////////

	// it is assumed that res is not the same object as A or B

	// could be implemented using sign checks
	
	// downward-mul: -( upward(a*(-b)) )
	
	// left end

	// forced execution order to 
	// obtain the desired ROUNDING effect
	double w1=B.left; w1=-w1; // no rounding
	
	// ROUNDING upward
	w1 *= A.left;
	
	// no rounding
	w1=-w1;
	// result is DOWNWARD-rounded A.left*B.left;
	
	// and the others
	double w2=B.right; w2=-w2; w2 *= A.left;  w2=-w2;
	double w3=B.left;  w3=-w3; w3 *= A.right; w3=-w3;
	double w4=B.right; w4=-w4; w4 *= A.right; w4=-w4;
	res.left=minimumDouble(w1,w2,w3,w4);
	
	// right end: only binary operation, so execution
	// order is clear and temporary results can be passed directly
	// to maximumDouble
	res.right=maximumDouble(
		A.left*B.left,
		A.left*B.right,
		A.right*B.left,
		A.right*B.right
	);
	
	// if resulting end point(s) are subnormal => adjust
	// if inf,nan => return error value -1
	IAVALIDATE_LEFT(res.left);
	IAVALIDATE_RIGHT(res.right);
	
	// everything worked, valid interval in variable res
	return 0;
}

int32_t custDiv_ZAB(
	CustInterval& res,
	CustInterval& A,
	CustInterval& B
) {
	// /////////////////////////////////////////
	// rounding mode is assumed UPWARD
	// /////////////////////////////////////////

	// it is assumed that res is not the same object as A or B

	// [a0..a1] / [b0..b1] = [a0..b0] * [1/b1..1/b0]
	// b must not contain zero
	
	// end-point test:
	// here also a check for inf/nan is possible without
	// speed-loss as the check for zero at the end-points
	// has to be done anyways
	if (fpclassify(B.left) != FP_NORMAL) return -1; // also checks for zero
	if (fpclassify(B.right)!= FP_NORMAL) return -1; // also checks for zero
	
	// zero within?
	if ( (B.left < 0.0) && (B.right > 0.0) ) return -1;
	
	CustInterval binv;
	// left end: downward 1/b.right = -( UPWARD( (-1)/B.right ) )
	binv.left=-1.0; // no rounding
	
	// ROUNDING upward
	binv.left /= B.right;
	
	binv.left = -binv.left; // no rounding
	// now binv.left is downward rounded 1/B.right
	
	// right end point: upward rounding
	binv.right=1.0/B.left;
	
	// subnormals can be moved to zero or DBL_min
	// i.e. binv can be a superset of the true 1/B
	// as basic interval operations are inclusion monotone,
	// multiplication below is a valid result
	IAVALIDATE_LEFT(binv.left)
	IAVALIDATE_RIGHT(binv.right)
	
	return custMul_ZAB(res,A,binv);
}
