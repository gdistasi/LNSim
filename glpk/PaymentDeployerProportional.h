
#include "PaymentDeployerMultipathExact.h"

using namespace std;

class PaymentDeployerProportional: public PaymentDeployerMultipathExact {

public:
	PaymentDeployerProportional(int numN, double P, int sourcet, int destinationt):
		PaymentDeployerMultipathExact(numN,P,sourcet,destinationt){};


	virtual int  RunSolver(std::vector<std::vector<long>> & flow, long & totalFee);

	void setLowerBound(int i, int j, long l) { lowerbound[pair<int,int>(i,j)] = l; }


protected:

	long getLowerBound(int i, int j);

	long feerate_perkw(int i, int j);

	/* lower bound for flows on channels -- upper bounds are given by channel capacities */
	map<pair<int,int>, long> lowerbound;

};
