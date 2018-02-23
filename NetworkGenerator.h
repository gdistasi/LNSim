/*
 * NetworkGenerator.h
 *
 *  Created on: 14 lug 2016
 *      Author: giovanni
 */

#ifndef NETWORKGENERATOR_H_
#define NETWORKGENERATOR_H_

#include <string>

#include "LightningNetwork.h"

class NetworkGenerator {
public:
	NetworkGenerator();
	virtual ~NetworkGenerator();

	static LightningNetwork * generate(int numNodes, double connDegree, double minFund, double maxFund,
							double sendingFee, double receivingFee, double positiveSlope, double negativeSlope, FeeType policy, int seed, int averageNumSlopes);


	static LightningNetwork * generateBase(int numNodes, double connectionProbability,
														   double minFund, double maxFund, double seed);

	static LightningNetwork * generateBaseFromFile(std::string filename);

	static LightningNetwork * generateOptimizedFee(LightningNetwork * baseNet,
														   long baseSendingFee_inMilliSatoshi,
														   double shigh, double slow, double seed);



	bool isConnected(LightningNetwork & net);

};

#endif /* NETWORKGENERATOR_H_ */
