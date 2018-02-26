/*
 * LightningNetwork.cpp
 *
 *  Created on: 06 lug 2016
 *      Author: giovanni
 */


#include "LightningNetwork.h"
#include "PaymentChannelEndPoint.h"
#include "PaymentChannel.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include "defs.h"


LightningNetwork::LightningNetwork() {
	// TODO Auto-generated constructor stubpay

}

LightningNetwork::~LightningNetwork() {

	for (auto p: channels){
		delete p;
	}

	for (auto p: nodes){
		delete p;
	}

}

void LightningNetwork::readFromFile(char* filename) {

	std::ifstream file;

	file.open(filename);

	std::string line;

	int numNodes, numChannels;

	while (std::getline(file, line))
	{
	    std::istringstream iss(line);
	    double resFundsA, resFundsB;

	    if (!(iss >> numNodes)) { break; } // error

	    for (int i=0; i<numNodes; i++){
	    	PaymentChannelEndPoint * e=new PaymentChannelEndPoint();
	    	e->id=i;
	    	nodes.push_back(e);
	    }

	    if (!(iss >> numChannels)) { break; } // error

	    int feePolicyT;

	    if (!(iss >> feePolicyT)) { break; } // error

	    switch (feePolicyT){
	    case 0:
	    	feePolicy=FeeType::FIXED;
	    	break;
	    case 1:
	    	feePolicy=FeeType::PROPORTIONAL;
	    	break;
	    case 2:
	    	feePolicy=FeeType::BILANCING;
	    	break;
	    }

	    for (int i=0; i<numChannels; i++){
		    std::istringstream iss(line);

	    	int a,b;
		    if (!(iss >> a >> b >> resFundsA >> resFundsB)) { break; } // error

		    PaymentChannel * pc=0;

		    switch (feePolicy){

		    	case FeeType::FIXED:
		    		double baseReceivingFee, baseSendingFee;
		    		if (!(iss >> baseSendingFee >> baseReceivingFee)) { break; } // error
		    		pc = new PaymentChannelFixedFee(nodes[a], nodes[b], resFundsA, resFundsB);
		    		((PaymentChannelFixedFee*)pc)->setBaseFees(baseSendingFee,baseReceivingFee);
		    		break;

		    	case FeeType::PROPORTIONAL:
		    		double propSendingFee, propReceivingFee;
		    		if (!(iss >> propSendingFee >> propReceivingFee)) { break; } // error
		    		pc = new PaymentChannelProportionalFee(nodes[a], nodes[b], resFundsA, resFundsB);
		    		((PaymentChannelProportionalFee*)pc)->setProportionalFees(propSendingFee, propReceivingFee);
		    		break;

		    	case FeeType::BILANCING:
		    		double positiveSlope, negativeSlope;
		    		PaymentChannelBalancingFee * pc = new PaymentChannelBalancingFee(nodes[a], nodes[b], resFundsA, resFundsB);
		    		if (!(iss >> positiveSlope >> negativeSlope)) { break; } // error
		    		((PaymentChannelBalancingFee*)pc)->setFeeSlopes(positiveSlope, negativeSlope);
		    		break;
		    }

    		channels.push_back(pc);
    		mapCh.insert(std::map<std::pair<PaymentChannelEndPoint *,PaymentChannelEndPoint *>,PaymentChannel *>::value_type(
    				std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[a],nodes[b]),pc));
	    }
	   }

	file.close();
}

ln_units LightningNetwork::totalFunds() {

	ln_units ret=0;

	for (PaymentChannel * ch: channels){
		ret+=ch->getResidualFundsA()+ch->getResidualFundsB();
		//std::cout << "CyCling" << channels.() << "ret " << ret << "\n";
#ifdef DEBUG
		ch->dump();
#endif
	}

	return ret;

}

void LightningNetwork::checkResidualFunds() const {
	for (PaymentChannel * ch : channels){
		if (ch->getResidualFundsA()<-MINIMUM_UNIT || ch->getResidualFundsB()<-MINIMUM_UNIT){
			std::cerr << "Residual funds of channel between " << ch->getEndPointA()->getId() << " and " <<
					ch->getEndPointB()->getId() << " is < 0: " << ch->getResidualFundsA() << " " << ch->getResidualFundsB() << "\n";
			exit(1);

		}
	}

	/*

	for (PaymentChannel * ch : channels){
		if (fabs(ch->getResidualFundsA())<MINIMUM_UNIT){
			ch->setResidualFundsA(0);
		}
		if (fabs(ch->getResidualFundsB())<MINIMUM_UNIT){
				ch->setResidualFundsA(0);
		}
	}*/

}

void LightningNetwork::makePayments(std::vector<std::vector<ln_units> > payments) {
	for (int i=0; i<getNumNodes(); i++){
		for (int j=0; j<getNumNodes(); j++){
			if (i==j || payments[i][j]==0) continue;


			//std::cout << "PAYINGGGGG \n";

			//std::cout << "i: " << i << " j: " << j << "\n";
			PaymentChannel * ch = getChannel(i,j);

			if (i<j){
					ch->PayB(payments[i][j]);
			}	else {
					ch->PayA(payments[i][j]);
			}
		}
	}
}



PaymentChannel * LightningNetwork::getChannel(int i, int j){
	PaymentChannel * pc=0;

	if (i<j){
		if ( mapCh.find(std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[i],nodes[j]))!=mapCh.end()){
			pc = mapCh[std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[i],nodes[j])];
		}
	} else {
		if ( mapCh.find(std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[j],nodes[i]))!=mapCh.end()){
			pc = mapCh[std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[j],nodes[i])];
		}
	}

	return pc;
}

void LightningNetwork::addPaymentChannel(PaymentChannel * pc, int idA, int idB){

	this->channels.push_back(pc);

	//std::cout << "Adding payment channel " << idA << " " << idB << "\n";

	if (idA<idB){
			mapCh.insert(std::map<std::pair<PaymentChannelEndPoint *,PaymentChannelEndPoint *>,PaymentChannel *>::value_type(
			std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[idA],nodes[idB]),pc));
	} else {
			mapCh.insert(std::map<std::pair<PaymentChannelEndPoint *,PaymentChannelEndPoint *>,PaymentChannel *>::value_type(
			std::pair<PaymentChannelEndPoint *, PaymentChannelEndPoint *>(nodes[idB],nodes[idA]),pc));
	}
}


