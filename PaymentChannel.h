/*
 * PaymentChannel.h
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#ifndef PAYMENTCHANNEL_H_
#define PAYMENTCHANNEL_H_


#include <vector>


#include "PaymentChannelEndPoint.h"

#include <assert.h>

#include "defs.h"



/*These are directional channels. A LN channel becomes two of these */  
class PaymentChannel {
public:
	PaymentChannel(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, ln_units resFundsA, ln_units resFundsB);

	virtual ~PaymentChannel();

	double getResidualFundsA() const {
		return residualFundsA;
	}

	void setResidualFundsA(ln_units residualFundsA) {
		this->residualFundsA = residualFundsA;
	}

	double getResidualFundsB() const {
		return residualFundsB;
	}

	void setResidualFundsB(ln_units residualFundsB) {
		this->residualFundsB = residualFundsB;
	}

	void setFeeCalculator(FeeCalculator * fc){ this->feeCalc=fc;}

	PaymentChannelEndPoint * getEndPointA() const { return A; }
	PaymentChannelEndPoint * getEndPointB() const { return B; }


	void PayA(double P);
	void PayB(double P);


	virtual bool doesFeeDependOnAmount(){ return false;}
	virtual void calcLinearizedFee(double paym_amount, double & sending, double  & receiving, bool reverse=false) const{};

	virtual millisatoshis calcFee(millisatoshis P, PaymentChannel * pc, bool reverse) { return feeCalc(P,pc,reverse);   }
)


protected:
	ln_units residualFundsA,residualFundsB;
	PaymentChannelEndPoint * A,*B;
	FeeCalculator * feeCalc;

};

class PaymentChannelFixedFee: public PaymentChannel {



public:
	PaymentChannelFixedFee(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB):
		PaymentChannel(A,B,resFundsA,resFundsB){

		baseSendingFee=0; baseReceivingFee=0;


	}


	void setBaseFees(double sf, double rf){
		baseSendingFee=sf;
		baseReceivingFee=rf;
	}

	virtual bool doesFeeDependOnAmount(){
		return false;
	}

	virtual void calcLinearizedFee(double paym_amount, double & sending, double & receiving, bool reverse=false) const {
		sending=baseSendingFee;
		receiving=baseReceivingFee;
	}



protected:
	double baseSendingFee,baseReceivingFee;

};


class PaymentChannelGeneralFees: public PaymentChannel {

public:


	PaymentChannelGeneralFees(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB):
		PaymentChannel(A,B,resFundsA,resFundsB){  }


	void setFeeFunctionA(double baseA, const std::vector<long> & starting_points, const std::vector<double> & coefficients){
		this->starting_pointsA=starting_points;
		this->coefficientsA=coefficients;
		this->baseFeeA=baseA;
	}

	void setFeeFunctionB(double baseA, const std::vector<long> & starting_points, const std::vector<double> & coefficients){
		this->starting_pointsB=starting_points;
		this->coefficientsB=coefficients;
		this->baseFeeB=baseA;
	}

	virtual bool doesFeeDependOnAmount(){
			return true;
	}
	
	virtual void calcLinearizedFee(double paym_amount, double & sending, double & receiving, bool reverse=false) const {
	}

	double getBaseFeeA(){ return baseFeeA; }


	double getBaseFeeB(){ return baseFeeB; }


	/* Points where the slope changes */
	std::vector<long> getPointsA(){
            return starting_pointsA;
    }
    
    std::vector<double> getSlopesA(){
            return coefficientsA;
    }


	/* Points where the slope changes */
	std::vector<long> getPointsB(){
            return starting_pointsB;
    }

    std::vector<double> getSlopesB(){
            return coefficientsB;
    }
	
    void setBaseFeeA(double base) {baseFeeA=base;}
    void setBaseFeeB(double base) {baseFeeB=base;}


	void addSlopeA(long start, double coeff);
   	void addSlopeB(long start, double coeff);


	//virtual void calcLinearizedFee(double paym_amount, double & sending, double & receiving, bool reverse=false) const {  }

protected:
	double baseFeeA;
	std::vector<long>  starting_pointsA;
	std::vector<double>  coefficientsA;

	double baseFeeB;
    std::vector<long>  starting_pointsB;
	std::vector<double>  coefficientsB;
    

};


class PaymentChannelProportionalFee: public PaymentChannel {

public:
	PaymentChannelProportionalFee(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB):
		PaymentChannel(A,B,resFundsA,resFundsB){}


	void setProportionalFees(double sf, double rf){
		proportionalSendingFee=sf;
		proportionalReceivingFee=rf;
	}

	virtual bool doesFeeDependOnAmount(){
			return true;
	}

	virtual void calcLinearizedFee(double paym_amount, double & sending, double & receiving, bool reverse=false) const {
			sending=proportionalSendingFee;
			receiving=proportionalReceivingFee;
	}

protected:
	double proportionalSendingFee, proportionalReceivingFee;

};

class PaymentChannelBalancingFee: public PaymentChannel {


public:


	PaymentChannelBalancingFee(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB):
		PaymentChannel(A,B,resFundsA,resFundsB){}

	void setFeeSlopes(double ps, double ns){
		positiveSlope=ps;
		negativeSlope=ns;
	}

	virtual bool doesFeeDependOnAmount(){
			return true;
	}

	virtual void calcLinearizedFee(double paym_amount, double & sending, double & receiving, bool reverse=false) const {
			if (!reverse){

				if (residualFundsA > residualFundsB){
					assert(residualFundsA-residualFundsB>=paym_amount);
					sending = positiveSlope;
					receiving = negativeSlope;
				} else {
					assert(-residualFundsA+residualFundsB>=paym_amount);
					sending = negativeSlope;
					receiving = positiveSlope;
				}
			} else {

				if (residualFundsB > residualFundsA){
					assert(residualFundsB-residualFundsA>=paym_amount);
					sending = positiveSlope;
					receiving = negativeSlope;
				} else {
					assert(-residualFundsB+residualFundsA>=paym_amount);
					sending = negativeSlope;
					receiving = positiveSlope;
				}
			}
	}

protected:
	double positiveSlope, negativeSlope;

};

#endif /* PAYMENTCHANNEL_H_ */
