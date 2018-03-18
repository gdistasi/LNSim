/*
 * MultipathHeuristic.cpp
 *
 *  Created on: 26/feb/2018
 *      Author: giovanni
 */

#include "MultipathHeuristic.h"
#include "glpk/PaymentDeployerProportional.h"
#include "defs.h"
#include "utils.h"
#include <limits>

MultipathHeuristic::~MultipathHeuristic() {
	// TODO Auto-generated destructor stub

}


int  MultipathHeuristic::RunSolver( Tflows & flow, long & totalFee){

	double bestFees = DBL_MAX;
	int bestNumberOfPaths=-1;
	vector<Tpath> bestPaths;

	for (int k=1; k<=numPaths; k++){

		std::cout << "Trying with " << k << " paths.\n";

		// solve the problem with k paths
		SinglePathSolver sp(*this);

		//the original amount is divided equally on the k paths
		sp.setAmount(payment/k);

		vector<Tpath> paths;
		long tFees=0;

		//let's find the k  paths.
		//We start by finding a best path for a payment of value  P/k on a single path...
		//and so on
		bool success=true;
		for (int i=1; i<=k; i++){

			long fee;
			Tflows  flows(numNodes,vector<ln_units>(numNodes));

			if (sp.RunSolver(flows, fee)!=0) {
				success=false;
				break;
			}

			printSolution(cout, string("Solution for path ") + to_string(i) + " of " + to_string(k), flows, fee);


			//std::cout << "Adding fee " <<fee << "\n";
			tFees+=fee;
			Tpath path = convertFlowsToPath(flows, source, destination);


			paths.push_back(path);

			//sp.removeUsedCapacity(path);
			sp.removeChannels(path);


		}

		if (k>1 && success && optimizationStep) {

				PaymentDeployerProportional pdp(numNodes, payment, source, destination, k);

				int pathN=1;
				for (auto pv: paths){
					for (auto p: pv){
						int from=p.first.first;
						int to=p.first.second;

						map<pair<int,int>, PaymentDeployer::PaymentChannel *> channelsNew = pdp.getChannels();

						if (channelsNew.find(pair<int,int>(from,to)) == channelsNew.end()){
							PaymentChannel * psource = channels[pair<int,int>(from,to)];
							vector< long> functionPoints = getFee(from,to).starting_points;

							int i=0;
							while (functionPoints[i] <= p.second){
								i++;
							}

							vector<long> coeff;
							coeff.push_back(getFee(from,to).coefficients[i-1]);

							vector<long> voidV;
							long baseFee = getFee(from,to).baseFee + calcFee(from,to,functionPoints[i-1]);
							pdp.AddPaymentChannel(from,to, functionPoints[i],0, baseFee , voidV,  coeff);

							if (functionPoints[i-1]>0){
								pdp.setLowerBound(from,to,pathN,functionPoints[i-1]);
								std::cout << "Setting lower bound between " << from << " and  " << to << " of path " << pathN << " to " << functionPoints[i-1] << "\n";
							}
							else
							{
								// the link must be used
								std::cout << "Setting lower bound between " << from << " and  " << to << " of path " << pathN << " to " << 1 << "\n";
								pdp.setLowerBound(from,to,pathN,1);
							}
							std::cout << "Setting upper bound between " << from << " and  " << to << " of path " << pathN << " to " << functionPoints[i] << "\n";

							pdp.setUpperBound(from,to,pathN,functionPoints[i]);

						}


					}
					pathN++;

				}

				for (int i=1; i<=k; i++){
					for (int x=0; x<numNodes; x++)
						for (int y=0; y<numNodes; y++){
							if (pdp.getLowerBound(x,y,i)==0){
								pdp.setUpperBound(x,y,i,0);
							}
						}
				}


				//Tflows flowOpt(numNodes, vector<long>(numNodes));

				vector<Tpath> pathsOpt;
				long feeOpt=0;

				if (pdp.RunSolver(pathsOpt, feeOpt)==0){

					/*long baseFees=0;
					for (auto pv: paths){
						for (auto p: pv){
							int from=p.first.first;
							int to=p.first.second;

							if (from != source)
								baseFees += fees[pair<int,int>(from,to)].baseFee;
						}
					}*/

					printSolution(cout, "Solution after optimization" , pathsOpt, feeOpt);


					if (feeOpt < tFees){
						std::cout << "Optimization step improved the solution. " << feeOpt << " vs " << tFees << " \n";
						tFees = feeOpt;
						paths = pathsOpt;
						//paths = convertFlowsToPaths(flowOpt, source, destination);
					} else {
						std::cout << "Optimization step has not improved the solution. " << feeOpt << " vs " << tFees << " \n";
					}

				} else {
					std::cerr<<"Optimization step failed.  \n";
					//feeOpt = std::numeric_limits<long>::max();
					//exit(1);
				}

		}


		if (success && tFees<bestFees){
			std::cout << "Total fee for k==" << k << " is " << tFees << "\n";
			bestNumberOfPaths = k;
			bestFees = tFees;
			bestPaths = paths;
		}

	}

	std::cerr << "Best number of paths " << bestNumberOfPaths << "\n";
	std::cerr << "Total fees " << bestFees << "\n";

	convertPathsToFlows(bestPaths, flow);
	totalFee = bestFees;

	if (bestNumberOfPaths!=-1) return 0;
	else return 1;

}
