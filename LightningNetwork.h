/*
 * LightningNetwork.h
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */

#include <vector>
#include <map>
#include <iostream>

#ifndef LIGHTNINGNETWORK_H_
#define LIGHTNINGNETWORK_H_


#include "PaymentChannelEndPoint.h"
#include "PaymentChannel.h"


class LightningNetwork {

public:
	LightningNetwork();
	virtual ~LightningNetwork();
	void readFromFile(char * file);

	int getNumNodes(){ return nodes.size(); }

	enum FeeType getFeePolicy() const {
		return feePolicy;

	}

	void checkResidualFunds() const;

	ln_units totalFunds();

	void setFeePolicy(enum FeeType feePolicy) {
		this->feePolicy = feePolicy;
	}

	const std::vector<PaymentChannel*>& getChannels()  {
		std::cout << "CHANNELs\n";
		return channels;
	}

	const std::vector<PaymentChannelEndPoint*>& getNodes() const {
		return nodes;
	}

	void makePayments(std::vector<std::vector<double>> );

	friend class NetworkGenerator;

protected:



	std::vector<PaymentChannelEndPoint *> nodes;
	std::vector<PaymentChannel *> channels;
	std::map<std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>, PaymentChannel *> mapCh;



	enum FeeType feePolicy;

};

#endif /* LIGHTNINGNETWORK_H_ */
