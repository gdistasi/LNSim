/*
 * PaymentChannel.h
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#ifndef PAYMENTCHANNEL_H_
#define PAYMENTCHANNEL_H_


#include "PaymentChannelEndPoint.h"

#include <assert.h>

enum FeeType : int { FIXED, PROPORTIONAL, BILANCING, GENERAL };


class PaymentChannel {
public:
	PaymentChannel(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB);

	virtual ~PaymentChannel();

	double getResidualFundsA() const {
		return residualFundsA;
	}

	void setResidualFundsA(double residualFundsA) {
		this->residualFundsA = residualFundsA;
	}

	double getResidualFundsB() const {
		return residualFundsB;
	}

	void setResidualFundsB(double residualFundsB) {
		this->residualFundsB = residualFundsB;
	}


	PaymentChannelEndPoint * getEndPointA() const { return A; }
	PaymentChannelEndPoint * getEndPointB() const { return B; }


	void PayA(double P);
	void PayB(double P);


	virtual bool doesFeeDependOnAmount()=0;
	virtual void calcLinearizedFee(double paym_amount, double & sending, double  & receiving, bool reverse=false) const =0;

protected:
	double residualFundsA,residualFundsB;
	PaymentChannelEndPoint * A,*B;

};

class PaymentChannelFixedFee: public PaymentChannel {



public:
	PaymentChannelFixedFee(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB):
		PaymentChannel(A,B,resFundsA,resFundsB){}


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
	PaymentChannelGeneralFees(PaymentChannelEndPoint * A, PaymentChannelEndPoint * B, double resFundsA, double resFundsB, double baseFee):
		PaymentChannel(A,B,resFundsA,resFundsB){this->base=baseFee;}


	void setSlopes(double base, std::vector<int> & starting_points, std::vector<double> & coefficients){
		this->starting_points=starting_points;
		this->coefficients=coefficients;
		this->base=base;
	}

	virtual bool doesFeeDependOnAmount(){
			return true;
	}
	
	void addSlope(int start, double coeff);

	virtual void calcLinearizedFee(double paym_amount, double & sending, double & receiving, bool reverse=false) const {  }

	void addSlope(int start, double coeff);

protected:
	double base;
	std::vector<int> & starting_points;
	std::vector<double> & coefficients;
};


class PaymentChannelProportionalFee: public PaymentChannel {
l
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
