/*
 * PaymentsGenerator.cpp
 *
 *  Created on: 13 lug 2016
 *      Author: giovanni
 */

#include "PaymentsGenerator.h"
#include <random>
#include "defs.h"


 NormalSizePoissonTimePaymentGenerator::~NormalSizePoissonTimePaymentGenerator() {
	delete size_dist;
	delete time_dist;
	delete src_dist;
	delete dst_dist;
}

void NormalSizePoissonTimePaymentGenerator::getNext(double& amount,
		double& time, int& source, int& destination) {

	 double interv = time_dist->operator() (generator);
	 time = last_time + interv;

	 last_time = time;

	 amount = 0;
	 do {
		 amount = size_dist->operator() (generator);
	 } while (amount<minAmount);

	 amount=mytrunc(amount);

	 source = src_dist->operator() (generator);

	 destination=source;

	 while (source==destination){
		 destination = dst_dist->operator() (generator);
	 }

}


