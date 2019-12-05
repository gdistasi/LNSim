/*
 * PaymentChannel.cpp
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#include "PaymentChannel.h"
#include "LightningNetwork.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include "defs.h"

PaymentChannel::PaymentChannel(PaymentChannelEndPoint * A,
		PaymentChannelEndPoint * B, ln_units resFundsA, ln_units resFundsB) {

	this->A=A;
	this->B=B;
	
    this->residualFundsA=resFundsA;
	this->residualFundsB=resFundsB;
    
    //updateFees(0);
    
	this->feeCalc=0;

    this->tbfu=0;
    
}


// update the fees that are used for routing (it is like such fees were announced on the network).
void PaymentChannel::announceFees(){
       
          anBaseFee=feeCalc->getBaseFee(residualFundsA,residualFundsB);
          anBaseFeeReverse=feeCalc->getBaseFee(residualFundsB,residualFundsA);
          
          slopesAn=feeCalc->getSlopes(residualFundsA,residualFundsB);
          slopesAnReverse=feeCalc->getSlopes(residualFundsB,residualFundsA);
          
          pointsAn=feeCalc->getPoints(residualFundsA,residualFundsB);
          pointsAnReverse=feeCalc->getPoints(residualFundsB,residualFundsA);
          
}   


double PaymentChannel::getTbfu(){
 return tbfu;   
}

void PaymentChannel::setTbfu(double tbfu){
        this->tbfu=tbfu;
}

void PaymentChannel::updateFees(double time){
    announceFees();
    this->lastTimeFeeUpdate = time;
}


double PaymentChannel::getLastTimeFeeUpdate(){
    return lastTimeFeeUpdate;
}

void PaymentChannel::PayA(ln_units P) {

#ifdef DEBUG
	std::cout << "Payment channel between " << this->A->getId() << " and " << this->B->getId() << "\n";
	std::cout << "Paying " << P << " to " << "A\n";
	std::cout << "Old balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";
#endif

	this->residualFundsB-=P;
	this->residualFundsA+=P;
    
#ifdef DEBUG
	std::cout << "New balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";
#endif

    //assert(this->residualFundsB>=0);
    //assert(this->residualFundsA>=0);
    
    
    if (this->residualFundsA<0){
        std::cerr << " ERROR: A " << this->residualFundsA << " B " << this->residualFundsB << " pay " << P << " \n";
        residualFundsA=0;
    }
    
    if (this->residualFundsB<0){
        std::cerr << " ERROR: A " << this->residualFundsA << " B " << this->residualFundsB << " pay " << P << " \n";
        residualFundsB=0;
    }
    

    

	//std::cout << "New balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";


}

void PaymentChannel::PayB(ln_units P) {
#ifdef DEBUG
	std::cout << "Payment channel between " << this->A->getId() << " and " << this->B->getId() << "\n";
	std::cout << "Paying " << P << " to " << "B\n";
	std::cout << "Old balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";
#endif


	this->residualFundsA-=P;
	this->residualFundsB+=P;

#ifdef DEBUG
	std::cout << "New balance A: " << this->residualFundsA << " B " << this->residualFundsB << "\n";
#endif

	//assert(this->residualFundsB>=0);
	//assert(this->residualFundsA>=0);
    
      
    if (this->residualFundsA<0){
	std::cerr << "ERROR: residual funds  A: " << this->residualFundsA << "\n";    
	    std::cerr << " A " << this->residualFundsA << " B " << this->residualFundsB << " pay " << P << " \n";
        residualFundsA=0;
    }
    
    if (this->residualFundsB<0){
	std::cerr << "ERROR: residual funds  B: " << this->residualFundsB << "\n";    
        std::cerr << " A " << this->residualFundsA << " B " << this->residualFundsB << " pay " << P << " \n";
        residualFundsB=0;
    }


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


std::vector<long> PaymentChannel::getPoints(bool reverse){
	if (!reverse)
		return feeCalc->getPoints(residualFundsAForFeeCalc,residualFundsBForFeeCalc);
	else
		return feeCalc->getPoints(residualFundsBForFeeCalc,residualFundsAForFeeCalc);
}

std::vector<long> PaymentChannel::getSlopes(bool reverse){
	if (!reverse ) {
		return feeCalc->getSlopes(residualFundsAForFeeCalc,residualFundsBForFeeCalc);
	} else {
		return feeCalc->getSlopes(residualFundsBForFeeCalc,residualFundsAForFeeCalc);
	}

}

    const std::vector<long> & PaymentChannel::getPointsAn(bool reverse){
           return reverse ? pointsAnReverse: pointsAn; 
    }
    

	const std::vector<long> & PaymentChannel::getSlopesAn(bool reverse){
           return reverse ? slopesAnReverse: slopesAn; 
    }
    
	const long PaymentChannel::getBaseFeeAn(bool reverse){
        return reverse ? anBaseFeeReverse: anBaseFee;
    }
    
    
    

long PaymentChannel::getBaseFee(bool reverse){
	return feeCalc->getBaseFee(residualFundsA, residualFundsB);
}

ln_units PaymentChannel::resFunds(int id) {
	assert(id==this->A->getId() || id==this->B->getId());

	if (A->getId()==id) return this->residualFundsA;
	else return residualFundsB;

}
