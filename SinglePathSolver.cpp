/*
 * SinglePathSolver.cpp
 *
 *  Created on: 23/feb/2018
 *      Author: giovanni
 */

#include "SinglePathSolver.h"

#include <limits>

#include <cassert>
#include <cmath>
#include <algorithm>

using namespace std;

#define DBL_MAX std::numeric_limits<double>::max()


int SinglePathSolver::constructPath(std::vector< std::vector<long> > & flow, long & totalFee, vector<Node> nodes){

	//find source
	Node * sourceN=&nodes[source];

	double fee=0;

	Node * nxt=sourceN;

	while ( nxt->getNext() != 0){
		long pay = round<long>(nxt->getNext()->getPaymentToArrive());
		flow[nxt->getId()][nxt->getNext()->getId()] = pay;
		std::cerr << "flow[" << nxt->getId() << ","<< nxt->getNext()->getId() << "]=" << pay << "\n";
		fee+=nxt->getDistance();
		nxt=nxt->getNext();
	}


	totalFee = round(fee);

	if (nxt->getId() != destination) return 1;

	return 0;

}

/* calc the distance from i to j: the amount of fee required to carry the payment P from i to j */
double SinglePathSolver::calcDistance(int i, int j, double payment){

	//vector<PaymentChannel *> chs = channelsByNode[i];
	assert(i!=j);

	double dis=DBL_MAX;

	if (channels.find(std::pair<int,int>(i,j))==channels.end() ||  channels[std::pair<int,int>(i,j)]->resFundsA < round(payment)){
		return dis;
	}

	if (i == source) return 0;

	if (fees.find(pair<int,int>(i, j))!=fees.end()){
		std::cout << "fee " << fees[pair<int,int>(i, j)].calcFee(payment) << "\n";
		dis = fees[pair<int,int>(i, j)].calcFee(payment);
	}

	return dis;
}

int  SinglePathSolver::RunSolver( std::vector< std::vector<long> > & flow, long & totalFee){

	vector<Node> nodes;
	vector<Node *> visited;
	vector<Node *> not_visited;

	for (int i=0; i<flow.size(); i++){
		if (i==destination){
			nodes.push_back(Node(i, 0, 0, payment));
		} else
			nodes.push_back(Node(i, 0, DBL_MAX, 0));
	}

	for (int i=0; i<flow.size(); i++)
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
					dis = calcDistance(n->getId(), vis->getId(), vis->getPaymentToGet());
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

		assert(minDis < DBL_MAX);

		//add the nearest to the visited set and erase from the non_visited
		visited.push_back(nearest);
		not_visited.erase(find(not_visited.begin(),not_visited.end(), nearest));

		if (nearest->getId() == source) {
			sourceReached = true;
			break;
		}

	}

	if (!sourceReached) return 1;
	else return constructPath(flow, totalFee, nodes);


}



