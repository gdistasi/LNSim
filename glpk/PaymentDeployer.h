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


    void setAmount(long am){ payment=am; }
    
    virtual int  RunSolver( std::vector< std::vector<long> > & flow, long & totalFee) = 0;

	virtual int  RunSolverOld(std::vector<std::vector<double>> & flow, double & totalFee){};
    
	unsigned long resFunds(int x,int y);

	class PaymentChannel {

			public:
				PaymentChannel(int At,int Bt, long resFundsAt, long resFundsBt):
					A(At),B(Bt),resFundsA(resFundsAt),resFundsB(resFundsBt){}

					//id of the endpoints
					int A,B;

					//residual funds of endpoints
					long resFundsA, resFundsB;
	};


protected:

    int numNodes;

    //the payment is expressed in cents of a satoshi
	long payment;
	int source;
	int destination;

    std::map<std::pair<int,int>, PaymentChannel> channels;

	enum solverErrors : int  {COULD_NOT_OPEN_DATA_FILE, COULD_NOT_OPEN_OUTPUT_FILE, PAYMENT_FAILED};
    
};


#define EPSILON             1E-15


#endif /* GLPK_PAYMENTDEPLOYER_H_ */

