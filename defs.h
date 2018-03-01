
#ifndef DEFS_H
#define DEFS_H

#include <vector>

using namespace std;


#define SATOSHI 1e-8
#define MINIMUM_UNIT SATOSHI
#define SATOSHIS_IN_BTC 100000000
#define MILLISATOSHIS_IN_BTC SATOSHIS_IN_BTC * 1000

// a path (i.e. in fact a flow allocation which can then also be multipath) and a fee
typedef vector< pair< pair<int, int>, long > > Tpath;
typedef pair< pair<int, int>, long > Tpath_el;

typedef vector< vector<long> > Tflows;

/* smallest unit that can be transferred in a channel - millisatoshis at the moment */
typedef long ln_units;

/* not used */
typedef  long millisatoshis;


enum ResolutionMethod : int { EXACT, HEURISTIC };

enum PaymentMethod : int { SINGLEPATH, ITERATIVE, MULTIPATH };

/* Methods used to automatically generate the fees for the channels: */
/* Base is the way to specify the fee based on a base fee plus a proportional part */
/* Fixed just the proportional part */
/* General is the way where the fee is a piecewise continous function */
/* General optimized is when the fee is a piecewise concave continous function */
/* Bilancing is equal to general in its form but the slopes are set to achieve a network wide objective */
enum FeeType : int { FIXED, BASE, GENERAL, PROPORTIONAL, BILANCING, GENERAL_OPTIMIZED };

// remove significant digits after the eigth
double mytrunc(double a);

#endif

