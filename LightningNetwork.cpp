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
			//exit(1);

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

	channelsByNode[idA].push_back(pc);
	channelsByNode[idB].push_back(pc);

}

double LightningNetwork::averageImbalance() {

	double imb=0;
	int numCh=0;

	for (auto ch: channels){
		imb = imb +  abs( ch->getResidualFundsA() - ch->getResidualFundsB() ) /2 ;
		numCh++;
	}

	return imb/this->totalFunds();

}

bool LightningNetwork::hasEnoughFunds(ln_units amount, int source, int paymentMethod) {
	bool has=false;

	vector<PaymentChannel *> pcs = channelsByNode[source];

	if (paymentMethod==PaymentMethod::SINGLEPATH){
		for (auto pc: pcs){
			if (pc->resFunds(source) >= amount){
				has=true;
			}
		}
	} else {
		ln_units sum=0;
		for (auto pc: pcs){
				sum+=pc->resFunds(source);
		}
		has = sum>=amount;

	}

	return has;

}

void LightningNetwork::dumpTopology(ostream& of) {
	of << nodes.size() << "\n";
	for (auto ch: channels){
		of << ch->getEndPointA()->getId() << " " << ch->getEndPointB()->getId() << " " << ch->getResidualFundsA() << " " <<
				ch->getResidualFundsB() << "\n";
	}
}

ln_units LightningNetwork::minFunds() {

	ln_units min=channels[0]->getResidualFundsA();

	for (auto pc:channels){
		if (pc->getResidualFundsA() < min)
			min = pc->getResidualFundsA();
		if (pc->getResidualFundsB() < min)
				min = pc->getResidualFundsB();
	}

	return min;

}

ln_units LightningNetwork::maxFunds() {
	ln_units max=channels[0]->getResidualFundsA();

		for (auto pc:channels){
			if (pc->getResidualFundsA() > max)
				max = pc->getResidualFundsA();
			if (pc->getResidualFundsB() < max)
					max = pc->getResidualFundsB();
		}

		return max;
}

void LightningNetwork::connect(int seed) {

	/*std::uniform_int_distribution<int> node_dist(0, nodes.size()-1);

	for (int i=0; i<vis.size(); i++){
		if (!vis[i]){


		}
	}
*/

}


void LightningNetwork::dfs(PaymentChannelEndPoint* v) {
    if (vis[v->getId()]) //don't visit the same node twice
        return;
    vis[v->getId()]=true;
    for (auto neigh: channelsByNode[v->getId()]){  //DFS to all the neighbors
    	PaymentChannelEndPoint * u = neigh->getEndPointA() == v ? neigh->getEndPointB() : neigh->getEndPointA();
    	if (!vis[u->getId()]) //check if you visited it before
            dfs(u);
    }
}

bool LightningNetwork::connected() {
	vis.resize(nodes.size());
	for (auto b: vis)
		b=false;

	dfs(nodes[0]);

	for (auto n: nodes){
	        if (!vis[n->getId()]) //DFS did not reach this node
	            return false;
	}

	return true;
}
