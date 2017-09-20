/*
 * PaymentChannelEndPoint.h
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#ifndef PAYMENTCHANNELENDPOINT_H_
#define PAYMENTCHANNELENDPOINT_H_

class PaymentChannelEndPoint {
public:
	PaymentChannelEndPoint();
	virtual ~PaymentChannelEndPoint();

	int getId() const {return id;}
	void setId(int id){ this->id=id; }

protected:
	int id;

	friend class LightningNetwork;

};

#endif /* PAYMENTCHANNELENDPOINT_H_ */
