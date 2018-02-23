/*
 * PaymentChannel.cpp
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#include "PaymentChannel.h"
#include "LightningNetwork.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include "defs.h"

PaymentChannel::PaymentChannel(PaymentChannelEndPoint * A,
		PaymentChannelEndPoint * B, ln_units resFundsA, ln_units resFundsB) {

	this->A=A;
	this->B=B;
	this->residualFundsA=resFundsA;
	this->residualFundsB=resFundsB;
	this->feeCalc=0;

}

void PaymentChannel::PayA(ln_units P) {

	std::cout << "Payment channel between " << this->A->getId() << " and " << this->B->getId() << "\n";
	std::cout << "Paying " << P << " to " << "A\n";
	std::cout << "Old balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";


	this->residualFundsB-=P;
	this->residualFundsA+=P;
	assert(this->residualFundsB>=0);
	assert(this->residualFundsA>=0);

	std::cout << "New balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";


}

void PaymentChannel::PayB(ln_units P) {
	std::cout << "Payment channel between " << this->A->getId() << " and " << this->B->getId() << "\n";
	std::cout << "Paying " << P << " to " << "B\n";

	std::cout << "Old balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";

	this->residualFundsA-=P;
	this->residualFundsB+=P;
	assert(this->residualFundsB>=0);
	assert(this->residualFundsA>=0);

	std::cout << "New balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";

}


void PaymentChannelGeneralFees::addSlopeA(long start, double coeff){
  coefficientsA.push_back(coeff);
  starting_pointsA.push_back(start);
}

void PaymentChannelGeneralFees::addSlopeB(long start, double coeff){
  coefficientsB.push_back(coeff);
  starting_pointsB.push_back(start);
}


PaymentChannel::~PaymentChannel(){

	if (feeCalc!=0){
		delete feeCalc;
	}

}

void PaymentChannel::dump(){

	std::cerr <<"Payment channel dump. ";
	std::cerr << "End points: " << A->getId() << " " << B->getId() << "\n";
	std::cerr << "ResidualFundsA " << residualFundsA << "; ResidualFundsB " << residualFundsB << "\n";

	if (feeCalc!=0)
		feeCalc->dump();
	else
		std::cerr << "FeeCalc not set.\n";

}


std::vector<unsigned long> PaymentChannel::getPoints(bool reverse){
	if (!reverse)
		return feeCalc->getPoints(residualFundsA,residualFundsB);
	else
		return feeCalc->getPoints(residualFundsB,residualFundsA);
}

std::vector<unsigned long> PaymentChannel::getSlopes(bool reverse){
	if (!reverse ) {
		return feeCalc->getSlopes(residualFundsA,residualFundsB);
	} else {
		return feeCalc->getSlopes(residualFundsB,residualFundsA);
	}

}

unsigned long PaymentChannel::getBaseFee(bool reverse){
	return feeCalc->getBaseFee(residualFundsA, residualFundsB);
}


