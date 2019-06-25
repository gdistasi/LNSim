/*
 * SinglePathSolver.cpp
 *
 *  Created on: 23/feb/2018
 *      Author: giovanni
 */

#include "MultipathHeuristic.h"
#include "SinglePathSolver.h"

#include <limits>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <cstring>

using namespace std;

SinglePathSolver::SinglePathSolver(const MultipathHeuristic & mp):PaymentDeployer(mp.numNodes, mp.payment, mp.source, mp.destination){

	//std::cout << "HERE\n";
	for (auto ch: mp.channels){
		int from = ch.first.first;
		int to = ch.first.second;
		assert(mp.fees.find(pair<int,int>(from,to))!=mp.fees.end());
		PaymentDeployer::PiecewiseLinearFee fee = mp.fees.find(pair<int,int>(from,to))->second;
		AddPaymentChannel(from, to, ch.second->resFundsA, ch.second->resFundsB, fee.baseFee, fee.starting_points, fee.coefficients);

	}
}

int SinglePathSolver::constructPath(std::vector< std::vector<long> > & flow, long & totalFee, vector<Node> nodes){

	//find source
	Node * sourceN=&nodes[source];

	double fee=0;

	Node * curr=sourceN;

	long paying = round<long>(curr->getNext()->getPaymentToArrive());

	while ( curr->getNext() != 0){
		long pay = round<long>(curr->getNext()->getPaymentToArrive());
		flow[curr->getId()][curr->getNext()->getId()] = pay;
#ifdef DEBUG
		std::cerr << "flow[" << curr->getId() << ","<< curr->getNext()->getId() << "]=" << pay << "\n";
#endif
		fee+=curr->getDistance();
		curr=curr->getNext();
	}


	totalFee = paying - payment;

	if (curr->getId() != destination) return 1;

	return 0;

}


int  SinglePathSolver::RunSolver( std::vector< std::vector<long> > & flow, long & totalFee){

	vector<Node> nodes;
	vector<Node *> visited;
	vector<Node *> not_visited;

	for (int i=0; i<numNodes; i++){
		if (i==destination){
			nodes.push_back(Node(i, 0, 0, payment));
		} else
			nodes.push_back(Node(i, 0, DBL_MAX, 0));
	}

	for (int i=0; i<numNodes; i++)
		if (i!=destination){
			not_visited.push_back(&nodes[i]);
		}


	visited.push_back( &nodes[destination] );

	bool sourceReached=false;

	/* until we don't visit the source */
	while (true){

		//update distance of nodes still not visited
		for (auto n: not_visited){
				double dis;
				for (auto vis: visited){
					dis = calcFee(n->getId(), vis->getId(), vis->getPaymentToGet());
					if ( dis < n->getDistance()){
						n->setDistance(dis);
						n->setPaymentToArrive(vis->getPaymentToGet() + dis);
						n->setNext(vis);
					}
				}
		}


		//select the nearest
		double minDis=DBL_MAX;
		Node * nearest=0;

		for (auto n: not_visited){
			if (n->getDistance() < minDis){
				nearest = n;
				minDis = n->getDistance();
			}
		}

		if (minDis == DBL_MAX) {
			//checkGraphIsConnected();
			return 1;
		}

		//add the nearest to the visited set and erase from the non_visited
		visited.push_back(nearest);
		not_visited.erase(find(not_visited.begin(),not_visited.end(), nearest));

		if (nearest->getId() == source) {
			sourceReached = true;
			break;
		}

	}

	assert(sourceReached);
	return constructPath(flow, totalFee, nodes);


}



