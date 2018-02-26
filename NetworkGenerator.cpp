 /*
 * NetworkGenerator.cpp
 *
 *  Created on: 14 lug 2016
 *      Author: giovanni
 */

#include "NetworkGenerator.h"
#include "FeeCalculator.h"
#include "defs.h"

#include <iostream>
#include <random>
#include <fstream>
#include <stdlib.h>
#include <math.h>

#include <cstdio>

#include  "utils.h"

NetworkGenerator::NetworkGenerator() {
	// TODO Auto-generated constructor stub

}

NetworkGenerator::~NetworkGenerator() {
	// TODO Auto-generated destructor stub
}



LightningNetwork * NetworkGenerator::generateOptimizedFee(LightningNetwork * baseNet,
													   long baseSendingFee_inMilliSatoshi,
													   double shigh, double slow, double seed){

	LightningNetwork * net = baseNet;

	//std::cerr << "Num channels " << net->getChannels().size() << "\n";

	for (PaymentChannel * ch: net->getChannels()){
		ch->setFeeCalculator(new FeeCalculatorOptimized(baseSendingFee_inMilliSatoshi, slow,shigh));
	}


	return net;

}


LightningNetwork * NetworkGenerator::generate(int numNodes, double connProb,
											  double minFund, double maxFund,
											  double sendingFee, double receivingFee,
											  double positiveSlope, double negativeSlope,
											  FeeType feePolicy, int seed, int averageNumSlopes) {

//	umNodes, connectionProbability, minFund, maxFund, sendingFee,
	//			receivingFee, positiveSlope, negativeSlope, feePolicy, seed, averageNumSlopes


			LightningNetwork * net= new LightningNetwork();

			for (int i=0; i<numNodes; i++){
					PaymentChannelEndPoint * n=new PaymentChannelEndPoint();
					net->nodes.push_back(n);
					n->setId(i);
			}

			std::default_random_engine generator(seed);
			std::uniform_real_distribution<double> dist(0,1);
			std::uniform_real_distribution<double> dist_funds(minFund,maxFund);

			std::uniform_int_distribution<int> num_slopes_distribution(0,3);
			std::uniform_int_distribution<int> slopes_distance_distribution(0,SATOSHI*1000000);

			net->feePolicy=feePolicy;

			for (int i=0; i<numNodes; i++)
				for (int j=i+1; j<numNodes; j++){
					//if (i==j) continue;
					double prob=dist(generator);

					if (prob<connProb){

						PaymentChannel * pc=0;

						switch (feePolicy){

						case FeeType::FIXED:{
							double baseReceivingFee, baseSendingFee;
							baseReceivingFee=receivingFee;
							baseSendingFee=sendingFee;
							pc = new PaymentChannelFixedFee(net->nodes[i], net->nodes[j], mytrunc(dist_funds(generator)), mytrunc(dist_funds(generator)));
							((PaymentChannelFixedFee*)pc)->setBaseFees(baseSendingFee,baseReceivingFee);
							break;
						}

						case FeeType::PROPORTIONAL:{
							double propSendingFee, propReceivingFee;
							propReceivingFee=receivingFee;
							propSendingFee=sendingFee;
							double fundsA,fundsB;
							fundsA=dist_funds(generator);
							fundsB=dist_funds(generator);
							pc = new PaymentChannelProportionalFee(net->nodes[i], net->nodes[j], mytrunc(fundsA), mytrunc(fundsB));
							((PaymentChannelProportionalFee*)pc)->setProportionalFees(propSendingFee, propReceivingFee);
							//std::cout << "FUNDS " << fundsA << " " << fundsB << "\n";
							break;
						}

						case FeeType::BILANCING:{
							PaymentChannelBalancingFee * pc = new PaymentChannelBalancingFee(net->nodes[i], net->nodes[j], mytrunc(dist_funds(generator)), mytrunc(dist_funds(generator)));
							((PaymentChannelBalancingFee*)pc)->setFeeSlopes(positiveSlope, negativeSlope);
							break;
						}

						case FeeType::GENERAL:{
							PaymentChannelGeneralFees * pc = new PaymentChannelGeneralFees(net->nodes[i], net->nodes[j], mytrunc(dist_funds(generator)),  mytrunc(dist_funds(generator)) );
							
							pc->setBaseFeeA(10);
							pc->setBaseFeeB(10);

                            int numSlopesA = num_slopes_distribution(generator);
                            int numSlopesB = num_slopes_distribution(generator);
                            
							int slope_startA=0;
                            int slope_startB=0;
                            
							for (int i=0; i<numSlopesA; i++){
								slope_startA = slope_startA + slopes_distance_distribution(generator);
								pc->addSlopeA(slope_startA,dist(generator));
							}
							for (int i=0; i<numSlopesB; i++){
								slope_startB = slope_startB + slopes_distance_distribution(generator);
								pc->addSlopeB(slope_startB,dist(generator));
							}
							
							break;
						}

						}

						net->channels.push_back(pc);
						net->mapCh.insert(std::map<std::pair<PaymentChannelEndPoint *,PaymentChannelEndPoint *>,PaymentChannel *>::value_type(
								std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(net->nodes[i],net->nodes[j]),pc));
					}
				}

			return net;
}

/**
mark all nodes as unvisited
for every node c, if this node is unvisited
create a new empty set of nodes, the current component
enqueue the this node for traversal
while there is any node t enqueued
remove this node from the queue
mark every unvisited neighbor as open and enqueue it for traversal
mark this node as traversed
add this node to the current component
close the current component, and add it to the set of components
If there is only one component, the graph is connected.
bool NetworkGenerator::isConnected(LightningNetwork & net){
*/

bool isConnected(LightningNetwork & net){

	std::vector<std::vector<PaymentChannelEndPoint *>> sets;

	for (PaymentChannelEndPoint * node: net.getNodes()){

		bool visited[net.getNumNodes()];



		if (!visited[node->getId()]){

			std::vector<PaymentChannelEndPoint *> newSet;
			newSet.push_back(node);


		}

	}

	return false;
}




LightningNetwork * NetworkGenerator::generateBase(int numNodes, double connProb, double minFund, double maxFund, double seed){

		LightningNetwork * net= new LightningNetwork();

		for (int i=0; i<numNodes; i++){
				PaymentChannelEndPoint * n=new PaymentChannelEndPoint();
				net->nodes.push_back(n);
				n->setId(i);
		}

		std::default_random_engine generator(seed);
		std::uniform_real_distribution<double> dist(0,1);
		std::uniform_real_distribution<double> dist_funds(minFund,maxFund);

		for (int i=0; i<numNodes; i++)
					for (int j=i+1; j<numNodes; j++){

						PaymentChannel * pc=0;

						//if (i==j) continue;
						double prob=dist(generator);

						if (prob<connProb){
							pc = new PaymentChannel(net->nodes[i], net->nodes[j],
																mytrunc(dist_funds(generator)), mytrunc(dist_funds(generator)));

							net->addPaymentChannel(pc, i, j);

#ifdef DEBUG
							pc->dump();
#endif
						}

		}

		return net;
}




LightningNetwork *  NetworkGenerator::generateBaseFromFile(string filename){

	LightningNetwork * net = new LightningNetwork();

	ifstream fpOut (filename);

	if (!fpOut.is_open()) {
		cout << "failed to open file: " << filename << endl;
		exit(1);
	}

	string line;

	getline (fpOut,line);

	int numNodes=convertTo(line);

	for (int i=0; i<numNodes; i++){
				PaymentChannelEndPoint * n=new PaymentChannelEndPoint();
				net->nodes.push_back(n);
				n->setId(i);
	}


	int from,to;
	long capA,capB;

	while (	getline (fpOut,line)){
		from = convertTo ( tokenize(line)[0]  );
		to = convertTo( tokenize(line)[1]  );
		capA = convertTo( tokenize(line)[2]  );
		capB = convertTo( tokenize(line)[3]  );

		PaymentChannel * pc = new PaymentChannel(net->nodes[from], net->nodes[to],capA, capB);

		net->addPaymentChannel(pc, from, to);

	}


	return net;

}

