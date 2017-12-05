
#include "PaymentDeployer.h"

using namespace std;

class PaymentDeployerProportional: public PaymentDeployer {

public:
	PaymentDeployerProportional(int numN, double P, int sourcet, int destinationt):
		PaymentDeployer(numN,P,sourcet,destinationt){};

	void AddPaymentChannel( int A, int B, double resFundsA, double resFundsB, double sfeeA );

	virtual ~PaymentDeployerProportional(){}

	virtual int  RunSolver(std::vector<std::vector<double>> & flow, double & totalFee);

	//int  RunSolverOld(std::vector<std::vector<double>> & flow, double & totalFee);

protected:
	double sendingFee(int x,int y);
	//double receivingFee(int x,int y);

	std::map< std::pair<int,int>, double> propFee;
};
