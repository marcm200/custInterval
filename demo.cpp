
/*

	simple interval struct with end-points double
	based on constant upward rounding, no
	rounding mode switch necessary during operations
	
	Marc Meidlinger
	May 2020
	
	NO WARRANTY OF CORRECT RESULTS (although I did my best to
	ensure correctness of the implementation)
	
	CAUTION: Every arithmetics routine custNNN_ZAB()
			 ASSUMES rounding mode is set to UPWARDS
			 
			 It is at the discretion of the user to
			 ensure this
	
	based on the article:

	Interval arithmetic with fixed rounding modw
	by S. Rump et al., 2016

*/

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "custint.h"

int32_t main(int32_t argc,char** argv) {
	// set rounding to upwards and store previous mode
	CustRoundingUpwards* rd=new CustRoundingUpwards;
	
	CustInterval a; // no initialization
	CustInterval b(-2.25); // 1-point interval
	CustInterval c(-12,1.25);
	CustInterval d(-64,128);
	
	printf("add [%.20lg..%.20lg] + [%.20lg..%.20lg] ",b.left,b.right,c.left,c.right);
	if (custAdd_ZAB(a,b,c) >= 0) printf(" = [%.20lg..%.20lg]\n",a.left,a.right);
	else printf("ERROR\n");
	
	printf("mul [%.20lg..%.20lg] * [%.20lg..%.20lg] ",c.left,c.right,d.left,d.right);
	if (custMul_ZAB(a,c,d) >= 0) printf(" = [%.20lg..%.20lg]\n",a.left,a.right);
	else printf("ERROR\n");

	printf("div [%.20lg..%.20lg] / [%.20lg..%.20lg] ",d.left,d.right,c.left,c.right);
	if (custDiv_ZAB(a,d,c) >= 0) printf(" = [%.20lg..%.20lg]\n",a.left,a.right);
	else printf("ERROR\n");
	
	// negative one tenth
	CustInterval e(1.0),f(-10.0); 
	printf("div [%.20lg..%.20lg] / [%.20lg..%.20lg] ",e.left,e.right,f.left,f.right);
	if (custDiv_ZAB(a,e,f) >= 0) printf("      = [%.20lg..%.20lg]\n",a.left,a.right);
	else printf("ERROR\n");
	
	// 1/-10 with rounding mode switches
	fesetround(FE_DOWNWARD);
	double w1=e.left / f.left;
	fesetround(FE_UPWARD);
	double w2=e.left / f.left;
	printf("1/-10 with rounding mode change: %.20lg..%.20lg\n",w1,w2);
	
	// follow a complex interval under z^2+c
	CustInterval x(0);
	CustInterval y(0);
	CustInterval Cre(0.25);
	CustInterval Cim(-1.0/65536.0,2.0/65536.0);
	printf("orbit ([%.20lg..%.20lg],[%.20lg..%.20lg]) under z^2+([%.20lg..%.20lg],[%.20lg..%.20lg]) ",
		x.left,x.right,y.left,y.right,
		Cre.left,Cre.right,Cim.left,Cim.right);
	
	int32_t error=0;
	
	// a higher loop value will reach infinity quite quickly
	// and result in an error
	for(int32_t i=0;i<683;i++) {
		CustInterval x2,y2,xy;
		error += custMul_ZAB(x2,x,x);
		error += custMul_ZAB(y2,y,y);
		error += custMul_ZAB(xy,x,y);
		
		CustInterval tmp1;
		error += custSub_ZAB(tmp1,x2,y2);
		error += custAdd_ZAB(x,tmp1,Cre);

		// as variables are passed by references
		// it is NOT recommended to do this:
		// (only after careful inspection of the called functions
		// where they impose changes)
		//          custSub_ZAB(x,x2,y2)
		// CAUTION: custAdd_ZAB(x,x,Cre)
		
		CustInterval tmp2;
		error += custAdd_ZAB(tmp2,xy,xy);
		error += custAdd_ZAB(y,tmp2,Cim);

		if (error != 0) {
			printf("ERROR at i=%i\n",i);
			exit(99);
		}
		
	}
	
	printf("\nresulted in ([%.20lg..%.20lg][%.20lg..%.20lg])\n",
		x.left,x.right,y.left,y.right);
	
	// reset the previous rounding mode
	delete rd; rd=NULL;
	
}

