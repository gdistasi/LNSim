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
#include <cassert>
#include <cmath>
#include <algorithm>

#include "defs.h"

using namespace std;

extern string modelsDirectory;

string exec(const char* cmd);

#define DBL_MAX std::numeric_limits<double>::max()



Tpath convertFlowsToPath(Tflows flows, int source, int destination);
//Tpath convertFromPathToFlows(Tflows flows);
void  convertPathsToFlows(vector<Tpath> path, Tflows & flows);


class PaymentDeployer {
    
public:
	PaymentDeployer(int numN, long P, int sourcet, int destinationt):
		numNodes(numN),payment(P),source(sourcet),destination(destinationt){};

	virtual ~PaymentDeployer();

	PaymentDeployer(const PaymentDeployer& pd);

	//PaymentDeployer(){}

	std::vector<long> getCoefficients(int x, int y);

	std::vector<long> getPoints(int x, int y);

	void AddPaymentChannel(int A, int B,  long resFundsA,  long resFundsB,  long baseFee, std::vector<long> sp, std::vector<long> cfs);

	long getBaseFee(int x, int y);

    void setAmount(long am){ payment=am; }
    
    virtual int  RunSolver(Tflows & flow, long & totalFee) = 0;

	virtual int  RunSolverOld(Tflows & flow, double & totalFee){};
    
	long resFunds(int x,int y);

	double calcFee(int i, int j, double payment);

	int removeUsedCapacity(Tpath path);
	int removeChannels(Tpath path);





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

	PiecewiseLinearFee getFee(int i, int j){
		return fees[pair<int,int>(i,j)];
	}

	map<pair<int, int>, PaymentDeployer::PaymentChannel *> & getChannels();

protected:

    int numNodes;
    //the payment is expressed in millisatoshis
	long payment;
	int source;
	int destination;

    map<pair<int,int>, PaymentChannel *> channels;
    map<int, vector<PaymentChannel *> > channelsByNode;
	map<pair<int,int>, PiecewiseLinearFee> fees;

	enum solverErrors : int  {COULD_NOT_OPEN_DATA_FILE, COULD_NOT_OPEN_OUTPUT_FILE, PAYMENT_FAILED};

};


#define EPSILON             1E-15


#endif /* GLPK_PAYMENTDEPLOYER_H_ */

