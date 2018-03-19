
#include "PaymentDeployerMultipathExact.h"

using namespace std;

class PaymentDeployerProportional: public PaymentDeployerMultipathExact {

public:
	PaymentDeployerProportional(int numN, double P, int sourcet, int destinationt, int numPaths):
		PaymentDeployerMultipathExact(numN,P,sourcet,destinationt){ this->numPaths = numPaths;  }

	virtual int  RunSolver(std::vector<std::vector<long>> & flow, long & totalFee);
	void setLowerBound(int i, int j, int pathN, long l);
	void setUpperBound(int i, int j, int pathN, long l);


	int  RunSolver(std::vector<Tpath> & paths, long & totalFee);

	long getLowerBound(int i, int j, int pathN);
	long getUpperBound(int i, int j, int pathN);



protected:
	int  parseOutputFile(string glpk_output, string outputFile, std::vector<Tpath> & paths, long & totalFee);
	long feerate_perkw(int i, int j);

	/* lower bound for flows on channels -- upper bounds are given by channel capacities */
	map<pair<int,int>, vector<long> > lowerbound;
	map<pair<int,int>, vector<long> > upperbound;

	int numPaths;

};
