/*
 * FeeCalculator.h
 *
 *  Created on: 12 lug 2016
 *      Author: giovanni
 */

#ifndef FEECALCULATOROLDA_H_
#define FEECALCULATOROLDA_H_

#include "defs.h"

#include <vector>

/*
class FeeCalculatorOld {
public:


	FeeCalculatorOld(FeeType feeTypeT, double baseFeeT):feeType(feeTypeT),baseFee(baseFeeT){};
	double getSendingFee(double payment, PaymentChannel & p, int A, int B);
	long getSendingFee2(ln_units payment, PaymentChannel & p, double basefee, double  slow, double shigh);
	virtual millisatoshis calcFee(millisatoshis P, PaymentChannel * pc, bool reverse)=0;
	virtual ~FeeCalculatorOld();

protected:
	FeeType feeType;
	double baseFee;


};*/

class PaymentChannel;

class FeeCalculator {
public:
	virtual millisatoshis calcFee(millisatoshis P, PaymentChannel * pc, bool reverse) = 0;
	virtual std::vector<unsigned long> getPoints(ln_units resFundsA, ln_units resFundsB) = 0;
	virtual std::vector<unsigned long> getSlopes(ln_units resFundsA, ln_units resFundsB) = 0;
	virtual unsigned long getBaseFee(ln_units resFundsA, ln_units resFundsB) = 0;
	virtual void dump() = 0;
};

class FeeCalculatorOptimized: public FeeCalculator {

public:
	FeeCalculatorOptimized(millisatoshis baseFeeT, millisatoshis slow, millisatoshis shigh):baseFee(baseFeeT),slow(slow),shigh(shigh){}
	millisatoshis calcFee(millisatoshis P, PaymentChannel * pc, bool reverse);
	std::vector<unsigned long> getPoints(ln_units resFundsA, ln_units resFundsB);
	std::vector<unsigned long> getSlopes(ln_units resFundsA, ln_units resFundsB);
	unsigned long getBaseFee(ln_units resFundsA, ln_units resFundsB) { return baseFee; }
	void dump();

private:
	millisatoshis slow, shigh;
	unsigned long baseFee;
};

#endif /* FEECALCULATOROLD_H_ */

