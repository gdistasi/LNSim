/*
 * PaymentsGenerator.h
 *
 *  Created on: 13 lug 2016
 *      Author: giovanni
 */

#ifndef PAYMENTSGENERATOR_H_
#define PAYMENTSGENERATOR_H_

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

using namespace std;

class PaymentsGenerator {
public:
	PaymentsGenerator(int numNodesT, int sourcesT, int destinationsT, int seedT):
		numNodes(numNodesT),sources(sourcesT),destinations(destinationsT),seed(seedT){};

	virtual void getNext(double & amount, double & time, int & source, int & destination)=0;

	void setMinAmount(double min){
		minAmount=min;
	}


	virtual ~PaymentsGenerator(){};



protected:
	int numNodes;
	int sources;
	int destinations;
	double seed;
	double minAmount;
};

#endif /* PAYMENTSGENERATOR_H_ */


class NormalSizePoissonTimePaymentGenerator : public PaymentsGenerator {

public:

	NormalSizePoissonTimePaymentGenerator(int numNodesT, int numSources, int numReceivers, int seedT,
										  double meanT, double varianceT, double intervalT):
											  PaymentsGenerator(numNodesT,numSources,numReceivers,seedT),generator(seed){
		size_dist = new std::normal_distribution<double>(meanT,varianceT);
		time_dist = new std::exponential_distribution<double>(intervalT);
		node_dist = new std::uniform_int_distribution<int>(0, numNodesT-1);

		int node;

		// finding senders
		while (senders.size()!=numSources){
			node=node_dist->operator() (generator);
			if ( std::find(senders.begin(),senders.end(),node)!=senders.end())
					continue;
			std::cerr << "New sender: " << node << "\n";
			senders.push_back(node);
		}

		// finding receivers
		while (receivers.size()!=numReceivers){
			node=node_dist->operator() (generator);
			if ( std::find(receivers.begin(),receivers.end(),node)!=receivers.end() || std::find(senders.begin(), senders.end(), node)!=senders.end())
					continue;
			std::cerr << "New receiver: " << node << "\n";

			receivers.push_back(node);
		}

		last_time = 0;
	}

	virtual void getNext(double & amount, double & time, int & source, int & destination);

	virtual ~NormalSizePoissonTimePaymentGenerator();


protected:
	std::default_random_engine generator;
	std::normal_distribution<double> * size_dist;
	std::exponential_distribution<double>  * time_dist;
	std::uniform_int_distribution<int> * node_dist;
	double seed;
	double last_time;

	vector<int> senders,receivers;
};
