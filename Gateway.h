/*
 * Gateway.h
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#ifndef GATEWAY_H_
#define GATEWAY_H_

#include "PaymentChannelEndPoint.h"

class Gateway: public PaymentChannelEndPoint {
public:
	Gateway();
	virtual ~Gateway();

protected:


};

#endif /* GATEWAY_H_ */
