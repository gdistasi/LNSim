/*
 * SinglePathSolver.h
 *
 *  Created on: 23/feb/2018
 *      Author: giovanni
 */

#ifndef SINGLEPATHSOLVER_H_
#define SINGLEPATHSOLVER_H_

#include "PaymentDeployer.h"
#include <vector>
#include <iostream>
using namespace std;

class MultipathHeuristic;


class SinglePathSolver: public PaymentDeployer {
public:
	//SinglePathSolver();
	//virtual ~SinglePathSolver();
	SinglePathSolver(int numN, long P, int sourcet, int destinationt):
			PaymentDeployer(numN,P,sourcet, destinationt){};


	SinglePathSolver(const MultipathHeuristic & mp);

    virtual int  RunSolver( std::vector< std::vector<long> > & flow, long & totalFee);


    class Node {
    public:

    	Node(int id, Node * next, double distance, double paymentToArrive):id(id),next(next),distance(distance){
    		this->paymentToGet  = paymentToArrive;
    	}


    	void setNext(Node * nx){ next=nx;}
    	Node * getNext() { return next; }
    	void setDistance(double distance) { this->distance = distance; }
    	double getDistance(){ return distance; }
    	double getPaymentToGet(){ return paymentToGet; }
    	double getPaymentToArrive(){ return paymentToGet; }
    	void setPaymentToArrive(double P) { paymentToGet = P; }
    	int getId(){return id;}

    private:
    	int id;
    	Node * next;
    	double distance;
    	double paymentToGet;
    };

private:
    int constructPath(std::vector< std::vector<long> > & flow, long & totalFee, vector<Node> nodes);
    double calcDistance(int i, int j, double payment);


};

#endif /* SINGLEPATHSOLVER_H_ */
