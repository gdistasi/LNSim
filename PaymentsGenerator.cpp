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

	 source = node_dist->operator() (generator);

	 destination=source;

	 while (source==destination){
		 destination = node_dist->operator() (generator);
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



