/*
 * PaymentsGenerator.cpp
 *
 *  Created on: 13 lug 2016
 *      Author: giovanni
 */

#include "PaymentsGenerator.h"
#include <random>
#include "defs.h"
#include "utils.h"

NormalSizePoissonTimePaymentGenerator::NormalSizePoissonTimePaymentGenerator(int numNodesT, int numSources, int numReceivers, int seedT,
										  double meanT, double varianceT, double intervalT):
											  generator(seed){

		numNodes=numNodesT;
		sources=numSources;
		destinations=numReceivers;

		size_dist = new std::normal_distribution<double>(meanT,varianceT);
		time_dist = new std::exponential_distribution<double>(intervalT);
		node_dist = new std::uniform_int_distribution<int>(0, numNodesT-1);
		source_dist = new std::uniform_int_distribution<int>(0, numSources-1);
		destination_dist = new std::uniform_int_distribution<int>(0, numSources-1);


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
			if ( std::find(receivers.begin(),receivers.end(),node)!=receivers.end() )
					continue;
			std::cerr << "New receiver: " << node << "\n";

			receivers.push_back(node);
		}

		last_time = 0;
		minAmount = 0;
	}


 NormalSizePoissonTimePaymentGenerator::~NormalSizePoissonTimePaymentGenerator() {
	delete size_dist;
	delete time_dist;
}



void NormalSizePoissonTimePaymentGenerator::getNext(ln_units& amount,
		double& time, int& source, int& destination) {

	 double interv = time_dist->operator() (generator);
	 time = last_time + interv;

	 last_time = time;

	 amount = 0;
	 do {
		 amount = size_dist->operator() (generator);
	 } while (amount<minAmount);

	 amount=mytrunc(amount);

	 source = senders[source_dist->operator() (generator)];

	 destination=source;

	 while (source==destination){
		 destination = receivers[destination_dist->operator() (generator)];
	 }

}

PaymentGeneratorFromFile::PaymentGeneratorFromFile(std::string filename){
	fpOut = new ifstream();
	fpOut->open(filename);


	if (!fpOut->is_open()) {
			cout << "failed to open file: " << filename << endl;
			exit(1);
	}

}


void PaymentGeneratorFromFile::getNext(ln_units& amount,
		double& time, int& source, int& destination){

	string line;


	if (getline(*fpOut, line)) {

		amount = convertTo(tokenize(line)[0]) * 1000;
		time = convertToDouble(tokenize(line)[1]);
		source = convertTo(tokenize(line)[2]);
		destination = convertTo(tokenize(line)[3]);

	} else {
		amount = 0;
	}
}

PaymentGeneratorFromFile::~PaymentGeneratorFromFile(){
	fpOut->close();
	delete fpOut;
}



