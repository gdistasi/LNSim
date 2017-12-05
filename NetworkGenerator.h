/*
 * NetworkGenerator.h
 *
 *  Created on: 14 lug 2016
 *      Author: giovanni
 */

#ifndef NETWORKGENERATOR_H_
#define NETWORKGENERATOR_H_

#include "LightningNetwork.h"

class NetworkGenerator {
public:
	NetworkGenerator();
	virtual ~NetworkGenerator();

	static LightningNetwork * generate(int numNodes, double connDegree, double minFund, double maxFund,
							double sendingFee, double receivingFee, double positiveSlope, double negativeSlope, FeeType policy, int seed, int averageNumSlopes);


	bool isConnected(LightningNetwork & net);

};

#endif /* NETWORKGENERATOR_H_ */
