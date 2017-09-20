/*
 * PaymentRequest.h
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#ifndef PAYMENTREQUEST_H_
#define PAYMENTREQUEST_H_

#include "PaymentChannelEndPoint.h"

class PaymentRequest {
public:
	PaymentRequest();
	virtual ~PaymentRequest();

	const PaymentChannelEndPoint& getDestination() const {
		return destination;
	}

	void setDestination(const PaymentChannelEndPoint& destination) {
		this->destination = destination;
	}

	const PaymentChannelEndPoint& getSource() const {
		return source;
	}

	void setSource(const PaymentChannelEndPoint& source) {
		this->source = source;
	}

	double getValue() const {
		return value;
	}

	void setValue(double value) {
		this->value = value;
	}

protected:
	PaymentChannelEndPoint source,destination;
	double value;

};

#endif /* PAYMENTREQUEST_H_ */
