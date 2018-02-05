/*
 * FeeCalculator.cpp
 *
 *  Created on: 12 lug 2016
 *      Author: giovanni
 */

#include "FeeCalculator.h"

FeeCalculator::~FeeCalculator() {
	// TODO Auto-generated destructor stub
}


double FeeCalculator::getSendingFee(double payment, PaymentChannel & p, int A, int B){


	if (feeType==FIXED){


	}

	return baseFee;

}


millisatoshis FeeCalculatorOptimized::calcFee(millisatoshis P, PaymentChannel * pc,  bool reverse=false){

	ln_units fundsA,fundsB;

	fundsA = !reverse ? pc->getResidualFundsA() : pc->getResidualFundsB();
	fundsB = reverse ? pc->getResidualFundsA() : pc->getResidualFundsB();

	ln_units imb = fundsA - fundsB;

	millisatoshis fee=0;

	if (fundsA > fundsB) {
		if ( P <= imb )
			fee = baseFee + slow * P;
	} else {
		fee = baseFee + slow * imb + (P-imb) * shigh;
	} else {
		fee = baseFee + P * shigh;
	}

	return fee;
}
