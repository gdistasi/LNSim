/*
 * PaymentsGenerator.cpp
 *
 *  Created on: 13 lug 2016
 *      Author: giovanni
 */

#include "PaymentsGenerator.h"
#include <random>
#include <set>
#include <algorithm>
#include "defs.h"
#include "utils.h"

using namespace std;

NormalSizePoissonTimePaymentGenerator::NormalSizePoissonTimePaymentGenerator(int numNodesT, int numSources, int numReceivers, int seedT,
										  double meanT, double varianceT, double intervalT, set<int> toAvoid):
											  generator(seedT),generatorTime(seedT){

		numNodes=numNodesT;
		sources=numSources;
		destinations=numReceivers;

		//std::cerr << "Seed " << seedT << "\n";

		size_dist = new std::normal_distribution<double>(meanT,varianceT);
		time_dist = new std::exponential_distribution<double>(intervalT);
		source_dist = new std::uniform_int_distribution<int>(0, numSources-1);
		destination_dist = new std::uniform_int_distribution<int>(0, numReceivers-1);

		std::cerr << "To avoid size " << toAvoid.size() << "\n";

		if (numSources > numNodes - toAvoid.size()){
			numSources = numNodes - toAvoid.size();
		}

		if (numReceivers > numNodes - toAvoid.size()){
			numReceivers = numNodes - toAvoid.size();
		}



		int node;

		std::vector<int> nodes;

		for (int i=0; i<numNodesT; i++){
			if (toAvoid.find(i)==toAvoid.end()){
				nodes.push_back(i);
			}
		}

		std::vector<int> nodesCopy=nodes;

		std::cerr << "node size " << nodes.size() << "\n";

		// finding senders
		while (senders.size()!=numSources){
			std::uniform_int_distribution<int> node_dist(0, nodes.size()-1);
			int nodeN = node_dist(generator);
			node = nodes[nodeN];
			//if (toAvoid.find(node)!=toAvoid.end()){
				nodes.erase(nodes.begin()+nodeN);
				senders.insert(node);
			//}
		}

		// finding receivers
		while (receivers.size()!=numReceivers){
			std::uniform_int_distribution<int> node_dist(0, nodesCopy.size()-1);
			int nodeN = node_dist(generator);
			node=nodesCopy[nodeN];
			//if (toAvoid.find(node)!=toAvoid.end()){
				nodesCopy.erase(nodesCopy.begin()+nodeN);
				receivers.insert(node);
			//}
		}

		std::cerr << "Senders: \n";
		for (auto node: senders){
			std::cerr << node << " ";
		}

		std::cerr << "\nReceivers: \n";
		for (auto node: receivers){
			std::cerr << node << " ";
		}
		std::cerr <<"\n";


		last_time = 0;
		minAmount = 0;
	}


 NormalSizePoissonTimePaymentGenerator::~NormalSizePoissonTimePaymentGenerator() {
	delete size_dist;
	delete time_dist;
}



void NormalSizePoissonTimePaymentGenerator::getNext(ln_units& amount,
		double& time, int& source, int& destination) {

	 double interv = time_dist->operator() (generatorTime);
	 time = last_time + interv;

	 last_time = time;

	 amount = 0;
	 do {
		 amount = size_dist->operator() (generator);
	 } while (amount<minAmount);

	 amount=mytrunc(amount);

	 set<int>::const_iterator it(senders.begin());
	 advance(it, source_dist->operator() (generator));
	 source=*it;

	 destination=source;

	 while (source==destination){
		 it=receivers.begin();
		 advance(it, destination_dist->operator() (generator));
		 destination=*it;
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

		time = convertToDouble(tokenize(line)[0]);
		amount = convertTo(tokenize(line)[1]);
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



