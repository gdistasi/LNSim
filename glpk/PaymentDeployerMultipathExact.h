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

#include "../PaymentDeployer.h"

using namespace std;

class PaymentDeployerMultipathExact: public PaymentDeployer {

public:
	PaymentDeployerMultipathExact(int numN, double P, int sourcet, int destinationt):PaymentDeployer(numN,P,sourcet,destinationt){};


	virtual int  RunSolver( std::vector< std::vector<long> > & flow, long & totalFee);


	virtual ~PaymentDeployerMultipathExact(){}


protected:
	int  parseOutputFile(string glpk_output, string outputFile, std::vector< std::vector<long> > & flow, long & totalFee);


};


#endif /* GLPK_PAYMENTDEPLOYER_H_ */
