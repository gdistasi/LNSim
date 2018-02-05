/*
 * FeeCalculator.h
 *
 *  Created on: 12 lug 2016
 *      Author: giovanni
 */

#ifndef FEECALCULATOR_H_
#define FEECALCULATOR_H_

#include "PaymentChannel.h"



class FeeCalculator {
public:


	FeeCalculator(FeeType feeTypeT, double baseFeeT):feeType(feeTypeT),baseFee(baseFeeT){};

	double getSendingFee(double payment, PaymentChannel & p, int A, int B);

	long getSendingFee2(ln_units payment, PaymentChannel & p, double basefee, double  slow, double shigh);

	virtual ~FeeCalculator();

protected:
	FeeType feeType;

	double baseFee;


};


class FeeCalculatorOptimized {

	FeeCalculatorOptimized(double baseFeeT, double slow, double shigh):baseFee(baseFeeT),slow(slow),shigh(shigh){}

	millisatoshis calcFee(millisatoshis P, PaymentChannel * pc, bool reverse);

	double baseFee, slow, shigh;

};

#endif /* FEECALCULATOR_H_ */

