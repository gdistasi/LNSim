/*
 * MultipathHeuristic.cpp
 *
 *  Created on: 26/feb/2018
 *      Author: giovanni
 */

#include "MultipathHeuristic.h"
#include "glpk/PaymentDeployerProportional.h"
#include "defs.h"

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
		//We start by finding a best path for a payment of value  P/k on a single path... FIX
		//and so on
		bool success=true;
		for (int i=1; i<=k; i++){

			long fee;
			Tflows  flows(numNodes,vector<ln_units>(numNodes));

			if (sp.RunSolver(flows, fee)!=0) {
				success=false;
				break;
			}


			//std::cout << "Adding fee " <<fee << "\n";
			tFees+=fee;
			Tpath path = convertFlowsToPath(flows, source, destination);


			paths.push_back(path);

			sp.removeUsedCapacity(path);



		}

		if (optimizationStep) {

				PaymentDeployerProportional pdp(numNodes, payment, source, destination);

				for (auto pv: paths){
					for (auto p: pv){
						int from=p.first.first;
						int to=p.first.second;

						map<pair<int,int>, PaymentDeployer::PaymentChannel *> channelsNew = pdp.getChannels();

						if (channelsNew.find(pair<int,int>(from,to)) == channelsNew.end()){
							PaymentChannel * psource = channels[pair<int,int>(from,to)];
							vector< long> functionPoints = getFee(from,to).starting_points;

							int i=0;
							while ( functionPoints[i] <= p.second ){
								i++;
							}

							vector<long> coeff;
							coeff.push_back(getFee(from,to).coefficients[i-1]);

							vector<long> voidV;
							pdp.AddPaymentChannel(from,to, functionPoints[i],0, 0, voidV,  coeff);
							pdp.setLowerBound(from,to,functionPoints[i-1]);
						}

					}
				}

				Tflows flowOpt(numNodes, vector<long>(numNodes));
				long feeOpt=0;

				if (pdp.RunSolver(flowOpt, feeOpt)==0){

					long baseFees=0;
					for (auto pv: paths){
						for (auto p: pv){
							int from=p.first.first;
							int to=p.first.second;

							if (from != source)
								baseFees += fees[pair<int,int>(from,to)].baseFee;
						}
					}

					if (feeOpt+baseFees < tFees){
						std::cout << "Optimization step improved the solution. " << feeOpt+baseFees << " vs " << tFees << " \n";
						//paths = convertFlowsToPaths(flowOpt, source, destination);
					} else {
						std::cout << "Optimization step has not improved the solution. " << feeOpt+baseFees << " vs " << tFees << " \n";
					}
				} else {
					std::cerr<<"Optimization step failed.  \n";
					exit(1);
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
