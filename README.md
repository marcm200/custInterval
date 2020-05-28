# custInterval
Interval mini library for the four basic arithmetic operations using constant upward rounding where downward rounding is simulated using several negations as described in:

"Interval arithmetic with fixed rounding mode
by S. Rump et al., 2016"

NO WARRANTY for correctness of the results.

## Key points:
- constant rounding mode, so no switching necessary (speed-up)
- double as end points throughout
- basic operations as direct function calls
- passing objects by reference
- intervals having subnormals as endpoints are enlarged to appropriate normal enclosures
- infinity and NaN result in error
- see file demo.cpp for an example

At program start, the struct CustRoundingUpwards must be created that sets the rounding mode to upwards. At its destruction,
the initial rounding mode is restored.

Operations are implemented as functions named custAdd_ZAB, custMul_ZAB, custSub_ZAB, custDiv_ZAB and all take three
arguments of type CustInterval: (result,term1,term2). The return value
is 0 in case of success, -1 in case of an error (infinity, NaN or division by zero-containing interval).

Functions assume that rounding mode is upwards and that the result variable is a distinct object than any of the
passed arguments. 


<br>May 2020
<br>Marc Meidlinger
<br>marcm200@freenet.de



