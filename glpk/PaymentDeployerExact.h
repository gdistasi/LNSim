/*
 * PaymentDeployer.h
 *
 *  Created on: 07 lug 2016
 *      Author: giovanni
 */

#ifndef GLPK_PAYMENTDEPLOYER_H_
#define GLPK_PAYMENTDEPLOYER_H_

#include <map>
#include <stdio.h>
#include <vector>

class PaymentDeployer {
public:
	PaymentDeployerExact(int numN, double P, int sourcet, int destinationt):
		numNodes(numN),payment(P),source(sourcet),destination(destinationt){};

	void AddPaymentChannel(int A, int B, double resFundsA, double resFundsB, std::vector<long> & sp, std::vector<double> & cfs);
	virtual ~PaymentDeployerExact();
	void setAmount(double am){ payment=am; }
	int  RunSolver(std::vector<std::vector<double>> & flow, double & totalFee);
	int  RunSolverOld(std::vector<std::vector<double>> & flow, double & totalFee);

protected:
	double resFunds(int A,int B);
	double sendingFee(int x,int y);
	double receivingFee(int x,int y);



	class PaymentChannel {

		public:
			PaymentChannel(int At, int Bt, long rfA, long rfB, std::vector<long> & sp, std::vector<double> & cfs):
				A(At),B(Bt),starting_points(sp),coefficients(cfs),resFundsA(rfA),resFundsB(rfB){}

//			std::vector<int> & starting_points;
//			std::vector<double> & coefficients;

				int A,B;
				long resFundsA, resFundsB;
				std::vector<long> starting_points;
				std::vector<double> coefficients;

			};


	int numNodes;
	double payment;
	int source;
	int destination;

	std::map<std::pair<int,int>, PaymentChannel> channels;

	enum solverErrors : int  {COULD_NOT_OPEN_DATA_FILE, COULD_NOT_OPEN_OUTPUT_FILE, PAYMENT_FAILED};

};

#define EPSILON             1E-15


#endif /* GLPK_PAYMENTDEPLOYER_H_ */
