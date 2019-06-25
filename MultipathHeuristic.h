/*
 * MultipathHeuristic.h
 *
 *  Created on: 26/feb/2018
 *      Author: giovanni
 */

#ifndef MULTIPATHHEURISTIC_H_
#define MULTIPATHHEURISTIC_H_

#include "SinglePathSolver.h"

class MultipathHeuristic: public PaymentDeployer {
public:
	MultipathHeuristic(int numN, long P, int sourcet, int destinationt, int numPaths, bool opStep):
	PaymentDeployer(numN,P,sourcet, destinationt){ this->numPaths=numPaths; optimizationStep=opStep; }

    virtual int  RunSolver( std::vector< std::vector<long> > & flow, long & totalFee) ;

    virtual ~MultipathHeuristic();

    void setNumPaths(int numP){ this->numPaths = numPaths; }

    void optimization(bool p) { optimizationStep=p; }

    friend class SinglePathSolver;

private:
    int numPaths;
    bool optimizationStep;

};

#endif /* MULTIPATHHEURISTIC_H_ */
