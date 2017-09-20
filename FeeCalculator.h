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


	virtual ~FeeCalculator();

protected:
	FeeType feeType;

	double baseFee;


};

#endif /* FEECALCULATOR_H_ */
