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

	NormalSizePoissonTimePaymentGenerator(int numNodesT, int sourcesT, int destinationsT, int seedT,
										  double meanT, double varianceT, double intervalT):
											  PaymentsGenerator(numNodesT,sourcesT,destinationsT,seedT),generator(seed){
		size_dist = new std::normal_distribution<double>(meanT,varianceT);
		time_dist = new std::exponential_distribution<double>(intervalT);
		src_dist = new std::uniform_int_distribution<int>(0, sources-1);
		dst_dist = 	new std::uniform_int_distribution<int>(0, destinations-1);
		last_time = 0;
	}

	virtual void getNext(double & amount, double & time, int & source, int & destination);

	virtual ~NormalSizePoissonTimePaymentGenerator();


protected:
	std::default_random_engine generator;
	std::normal_distribution<double> * size_dist;
	std::exponential_distribution<double>  * time_dist;
	std::uniform_int_distribution<int> * src_dist, * dst_dist;
	double seed;
	double last_time;

};
