
#ifndef DEFS_H
#define DEFS_H


#define SATOSHI 1e-8
#define MINIMUM_UNIT SATOSHI

/* Base is the way to specify the fee based on a base fee plus a proportional part */
/* Fixed just the proportional part */
/* General is the way where the fee is a piecewise continous function */
/* Bilancing is equal to general in its form but the slopes are set to achieve a network wide objective */

enum FeeType : int { FIXED, BASE, GENERAL, PROPORTIONAL, BILANCING };

enum ResolutionMethod : int { EXACT, HEURISTIC };

enum PaymentMethod : int { SINGLEPATH, ITERATIVE, MULTIPATH };

// remove significant digits after the eigth
double mytrunc(double a);

#endif
