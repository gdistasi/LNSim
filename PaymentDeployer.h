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


extern std::string modelsDirectory;

std::string exec(const char* cmd);

class PaymentDeployer {
    
public:
	PaymentDeployer(int numN, long P, int sourcet, int destinationt):
		numNodes(numN),payment(P),source(sourcet),destination(destinationt){};

	virtual ~PaymentDeployer();

	std::vector<long> getCoefficients(int x, int y);

	std::vector<long> getPoints(int x, int y);

	void AddPaymentChannel(int A, int B,  long resFundsA,  long resFundsB,  long baseFee, std::vector<long> sp, std::vector<long> cfs);

	long getBaseFee(int x, int y);

    void setAmount(long am){ payment=am; }
    
    virtual int  RunSolver( std::vector< std::vector<long> > & flow, long & totalFee) = 0;

	virtual int  RunSolverOld(std::vector<std::vector<double>> & flow, double & totalFee){};
    
	long resFunds(int x,int y);

	/* directional channel */
	class PaymentChannel {

			public:
				PaymentChannel(int At,int Bt, long resFundsAt, long resFundsBt):
					A(At),B(Bt),resFundsA(resFundsAt),resFundsB(resFundsBt){}

					//id of the endpoints
					int A,B;

					//residual funds of endpoints
					long resFundsA, resFundsB;
	};

	class PiecewiseLinearFee {
		public:
			PiecewiseLinearFee(long baseFee, std::vector<long> starting_points, std::vector<long> coefficients):
				baseFee(baseFee),coefficients(coefficients),starting_points(starting_points){ }

			PiecewiseLinearFee(){ baseFee=0; }

			double calcFee(long payment);

			public:
				long baseFee;
				std::vector<long> starting_points;
				std::vector<long> coefficients;

	};



protected:

    int numNodes;

    //the payment is expressed in satoshi
	long payment;
	int source;
	int destination;

    std::map<std::pair<int,int>, PaymentChannel *> channels;

    std::map<int, std::vector<PaymentChannel *> > channelsByNode;

	enum solverErrors : int  {COULD_NOT_OPEN_DATA_FILE, COULD_NOT_OPEN_OUTPUT_FILE, PAYMENT_FAILED};

	std::map<std::pair<int,int>, PiecewiseLinearFee> fees;
};


#define EPSILON             1E-15


#endif /* GLPK_PAYMENTDEPLOYER_H_ */

