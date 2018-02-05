/*
 * PaymentDeployer.h
 *
 *  Created on: 07 lug 2016
 *      Author: giovanni
 */

#ifndef GLPK_PAYMENTDEPLOYEREX_H_
#define GLPK_PAYMENTDEPLOYEREX_H_

#include <map>
#include <stdio.h>
#include <vector>
#include <string.h>

#include "PaymentDeployer.h"

using namespace std;

class PaymentDeployerExact: public PaymentDeployer {

public:

	PaymentDeployerExact(int numN, double P, int sourcet, int destinationt):PaymentDeployer(numN,P,sourcet,destinationt){};

	void AddPaymentChannel(int A, int B, long resFundsA, long resFundsB, std::vector<long> sp, std::vector<double> cfs);

	int  RunSolver(std::vector<std::vector<double>> & flow, double & totalFee);

	std::vector<double> getCoefficients(int x, int y){ return fees[pair<int,int>(x,y)].coefficients; }
	std::vector<long> getPoints(int x, int y){ return fees[pair<int,int>(x,y)].starting_points; }

	virtual ~PaymentDeployerExact(){}

	class PiecewiseLinearFee {

	public:

		public:
			std::vector<long> starting_points;
			std::vector<double> coefficients;

	};

protected:

	std::map<std::pair<int,int>, PiecewiseLinearFee> fees;

};


#endif /* GLPK_PAYMENTDEPLOYER_H_ */
