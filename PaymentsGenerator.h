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
#include <string>
#include <set>
#include "defs.h"

using namespace std;

class PaymentsGenerator {
public:
	//PaymentsGenerator(int numNodesT, int sourcesT, int destinationsT, int seedT):
	//	numNodes(numNodesT),sources(sourcesT),destinations(destinationsT),seed(seedT){};

	virtual ~PaymentsGenerator(){};
	virtual void getNext(ln_units & amount, double & time, int & source, int & destination)=0;

	//nodes to not be selected as sources or destinations
	//virtual void toAvoid(set<int> s)=0;

};

class PaymentGeneratorFromFile: public PaymentsGenerator {
public:
	PaymentGeneratorFromFile(std::string filename);
	virtual void getNext(ln_units & amount, double & time, int & source, int & destination);
	//virtual void toAvoid(set<int> s);
	virtual ~PaymentGeneratorFromFile();

private:
	ifstream * fpOut;

};

class NormalSizePoissonTimePaymentGenerator : public PaymentsGenerator {

public:

	NormalSizePoissonTimePaymentGenerator(int numNodesT, int numSources, int numReceivers, int seedT,
										  double meanT, double varianceT, double intervalT, set<int> toAvoid);

	virtual void getNext(ln_units & amount, double & time, int & source, int & destination);

	virtual ~NormalSizePoissonTimePaymentGenerator();

	void setMinAmount(double min){
			minAmount=min;
	}


protected:
	std::default_random_engine generator;
	std::normal_distribution<double> * size_dist;
	std::exponential_distribution<double>  * time_dist;
	std::uniform_int_distribution<int> * node_dist;
	std::uniform_int_distribution<int> * source_dist;
	std::uniform_int_distribution<int> * destination_dist;

	double last_time;

	set<int> senders,receivers;
	//set<int> toAvoid;

	int numNodes;
	int sources;
	int destinations;
	double minAmount;
};

#endif /* PAYMENTSGENERATOR_H_ */

