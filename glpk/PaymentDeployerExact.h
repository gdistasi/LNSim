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

	void AddPaymentChannel(int A, int B, unsigned long resFundsA, unsigned long resFundsB, unsigned long baseFee, std::vector<long> sp, std::vector<unsigned long> cfs);

	virtual int  RunSolver( std::vector< std::vector<long> > & flow, long & totalFee);

	void AddPaymentChannel(int A, int B, unsigned long resFundsA, unsigned long resFundsB, unsigned long baseFee, std::vector<unsigned long> sp, std::vector<unsigned long> cfs);

	std::vector<unsigned long> getCoefficients(int x, int y);
	std::vector<unsigned long> getPoints(int x, int y);
	unsigned long getBaseFee(int x, int y);

	virtual ~PaymentDeployerExact(){}

	class PiecewiseLinearFee {
	public:
		PiecewiseLinearFee(unsigned long baseFee, std::vector<unsigned long> starting_points, std::vector<unsigned long> coefficients):
			baseFee(baseFee),coefficients(coefficients),starting_points(starting_points){ }

		PiecewiseLinearFee(){ baseFee=0; }

		public:
			unsigned long baseFee;
			std::vector<unsigned long> starting_points;
			std::vector<unsigned long> coefficients;

	};

protected:
	std::map<std::pair<int,int>, PiecewiseLinearFee> fees;

};


#endif /* GLPK_PAYMENTDEPLOYER_H_ */
