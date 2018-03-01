
#include "LightningNetwork.h"
//#ifdef MAIN
#include "PaymentsGenerator.h"
#include "PaymentDeployer.h"
#include "NetworkGenerator.h"

#include <cmath>
#include <iostream>
#include "defs.h"
#include <string.h>
#include <stdio.h>
#include "Statistics.h"

#include <typeinfo>

#include "SinglePathSolver.h"
#include "MultipathHeuristic.h"

#include <getopt.h>
#include "glpk/PaymentDeployerMultipathExact.h"
#include "utils.h"

using namespace std;

string modelsDirectory="glpk";


double findSmallestImbalance(std::vector<PaymentChannel *> channels){
	double ret=abs(channels[0]->getResidualFundsA()-channels[0]->getResidualFundsB());

	for (const PaymentChannel * ch: channels){
		if (abs(ch->getResidualFundsA()-ch->getResidualFundsB()) < ret){
			ret=abs(ch->getResidualFundsA()-ch->getResidualFundsB());
		}
	}

	return ret;

}

void checkPayment(std::vector<std::vector<ln_units>> flows, ln_units pay, ln_units fee, int src, int dst){

	std::cout << "Checking payment...\n";

	ln_units out=0,in=0;

	for (int i=0; i<(int)flows.size(); i++){
		out+=mytrunc(flows[src][i]);
		in+=mytrunc(flows[i][dst]);
	}

	std::cout << "...";


	if (fabs(out-in-fee)>MINIMUM_UNIT){
		std::cerr<<"Approximation error (out-in-fee): " << fabs(out-in-fee) <<"\n";
	}
	if (fabs(in-pay)>MINIMUM_UNIT){
		std::cerr<<"Approximation error (in-pay): " << fabs(in-pay) <<"\n";

	}
	//std::cout << "PAYING: " << pay << " OUT: " << out << " IN: "<< in << " FEE: "<< fee << " DIFF: " << out-in << "\n";


}




int main(int argc, char * * argv){

	int seed=0.1;
	ln_units meanSizePayments=1000 * 1000;
	double variancePayments=500;
	double intervalPayments=1;
	double totalTime=60; //one day
	int numNodes=2;
	int numSources=1;
	int numDestinations=1;
	double connectionProbability=0.1;

	ln_units minFund=0.01 * MILLISATOSHIS_IN_BTC;
	ln_units maxFund=0.167 * MILLISATOSHIS_IN_BTC;

	double sendingFee=1000 ;
	double receivingFee=0;
	//double positiveSlope=sendingFee;
	//double negativeSlope=sendingFee*0.01;


	/* in millisatoshis */
	long basefee=1000;

	/* in millisatoshis per kw (kiloweight - 1000 satoshis) transferred */
	long feerate_low=10;
	long feerate_high=30;

	/* in satoshis */
	//ln_units baseFee=1;

	int maxPayments=1;
	string networkFile="";
	string paymentsFile="";


	int averageNumSlopes=3;

	int numPaths=3;
    
    ResolutionMethod resMethod=ResolutionMethod::EXACT;
    FeeType feePolicy=FeeType::GENERAL;
    PaymentMethod payMethod=PaymentMethod::SINGLEPATH;

    bool hOptimization=true;

	//handling of options
	while (1)
	    {
	      static struct option long_options[] =
	        {
	          {"meanSizePayments",     required_argument,       0, 's'},
	          {"variancePayments",  required_argument,       0, 'v'},
	          {"intervalPayments",  required_argument, 0, 'i'},
	          {"totalTime",  required_argument, 0, 't'},
	          {"numNodes",    required_argument, 0, 'n'},
			  {"numSources",    required_argument, 0, 'q'},
			  {"numDestinations",    required_argument, 0, 'w'},
			  {"connectionProbability",    required_argument, 0, 'c'},
	          {"minFundsChannels",    required_argument, 0, 'm'},
			  {"maxFundsChannels",    required_argument, 0, 'M'},
			  {"modelsDirectory",   required_argument, 0, 'd'},
              {"feePolicy", required_argument, 0, 'f'},
			  {"resolutionMethod", required_argument, 0, 'r'},
			  {"paymentMethod", required_argument, 0, 'p'},
			  {"numPaths", required_argument, 0, 'R'},
			  {"maxPayments", required_argument, 0, 'P'},
			  {"networkFromFile", required_argument, 0, 'N'},
			  {"paymentsFromFile", required_argument, 0, 'Q'},
			  {"feerateLow", required_argument, 0, 'l'},
			  {"feerateHigh", required_argument, 0, 'h'},
			  {"heuristicOptimization", required_argument, 0, 'O'},
			  {"seed",   required_argument, 0, 'S'},

	          {0, 0, 0, 0}
	        };
	      /* getopt_long stores the option index here. */
	      int option_index = 0;

	      int c = getopt_long (argc, argv, "s:v:i:t:n:p:m:M:r:f:P:",
	                       long_options, &option_index);

	      /* Detect the end of the options. */
	      if (c == -1)
	        break;

	      switch (c)
	        {
	        case 0:
	          /* If this option set a flag, do nothing else now. */
	          if (long_options[option_index].flag != 0)
	            break;
	          //printf ("option %s", long_options[option_index].name);
	          //if (optarg)
	           // printf (" with arg %s", optarg);
	          //printf ("\n");
	          break;

	        case 's':
	        	meanSizePayments=atof(optarg);
	        	break;

	        case 'P':
	  	        	maxPayments=atoi(optarg);
	  	        	break;

	        case 'N':
	    	  	    networkFile=string(optarg);
	    	  	    break;
	        case 'R':
   	    	  	    numPaths=atoi(optarg);
  	    	  	    break;
	        case 'Q':
	    	  	    paymentsFile=string(optarg);
	    	  	    break;

	        case 'O':
	        	   	hOptimization=atoi(optarg)==1;
	        	    break;

	        case 'v':
	        	variancePayments=atof(optarg);
	        	break;

	        case 'l':
	 	        	feerate_low=atoi(optarg);
	 	        	break;
	        case 'h':
	 	        	feerate_high=atoi(optarg);
	 	        	break;
	        case 'i':
	        	intervalPayments=atof(optarg);
	        	break;

	        case 't':
				totalTime=atof(optarg);
	        	break;

	        case 'n':
	        	numNodes = atoi(optarg);
	        	break;

	        case 'c':
	        	connectionProbability = atof(optarg);
	        	break;

	        case 'm':
	        	minFund=atof(optarg);
	        	break;

	        case 'M':
	        	maxFund=atof(optarg);
	        	break;

	        case 'd':
	        	modelsDirectory=string(optarg);
	        	break;
	        case 'S':
	        	seed=atof(optarg);
	        	break;

	        case 'q':
	        	numSources=atoi(optarg);
	        	break;
	        case 'w':
	        	numDestinations=atoi(optarg);
	        	break;
                
            case 'f':
                if (strcmp("base", optarg)==0){
                    feePolicy = FeeType::BASE;
                } else if (strcmp("general", optarg)==0){
                    feePolicy = FeeType::GENERAL;
                } else if  (strcmp("general_optimized", optarg)==0){
                    feePolicy = FeeType::GENERAL_OPTIMIZED;
                } else {
                  cerr << "Fee type " << optarg << "  not supported. \n";
                  abort();
                }	        
                break;    
                
            case 'r':
                if (strcmp("exact", optarg)==0){
                    resMethod=ResolutionMethod::EXACT;
                } else if (strcmp("heuristic", optarg)==0){
                    resMethod=ResolutionMethod::HEURISTIC;
                } else {
                  cerr << "Resolution method " << optarg << " not supported.\n";
                  abort();
                }
                break;
                

            case 'p':
                if (strcmp("sp", optarg)==0){
                    payMethod=PaymentMethod::SINGLEPATH;
                } else if (strcmp("mp", optarg)==0){
                	payMethod=PaymentMethod::MULTIPATH;
                } else {
                  cerr << "Payment method " << optarg << " not supported.\n";
                  abort();
                }
                break;

	        case '?':
	          /* getopt_long already printed an error message. */
	        	exit(1);
	          break;

	        default:
	          abort ();
	        }
	    }

	LightningNetwork * net=0;

	std::cerr << "All values are expressed in millisatoshis. Input files are in satoshis.\n";
	std::cerr << "Generating network...\n";

	if (networkFile!=""){
		net = NetworkGenerator::generateBaseFromFile(networkFile);
	} else {
		net = NetworkGenerator::generateBase(numNodes, connectionProbability, minFund, maxFund, seed);
	}

	if (feePolicy = GENERAL_OPTIMIZED){
		net=NetworkGenerator::generateOptimizedFee(net, sendingFee ,
													  feerate_high , feerate_low, seed);
	} else {
//		net = new NetworkGenerator::generate(numNodes, connectionProbability, minFund, maxFund, sendingFee,
//		receivingFee, positiveSlope, negativeSlope, feePolicy, seed, averageNumSlopes);
		std::cerr << "Not supported! " << endl;
		exit(1);
	}

	std::cerr << "Done.\n";

	std::cerr << "Initial funds: " << net->totalFunds() << std::endl;

	PaymentsGenerator * paymGen=0;

	if (paymentsFile!=""){
		paymGen = new PaymentGeneratorFromFile(paymentsFile);
	} else {
		paymGen = new NormalSizePoissonTimePaymentGenerator(net->getNumNodes(), numSources, numDestinations,
										  seed, meanSizePayments, variancePayments, intervalPayments);
	}

	double time;
	ln_units amount;
	int src,dst;

	long initFunds = net->totalFunds();

	Stats stats;

	do {

		net->checkResidualFunds();

		std::cerr << "Getting next payment info...";
		paymGen->getNext(amount,time,src,dst);

		stats.numPayments+=1;
		std::cerr << "Processing next payment - amount: " << amount << " time: " << time << " Src: " << src << " Dst: " << dst << "\n";

		ln_units totalFee=0;

        PaymentDeployer * pd;
        
        if (    resMethod==ResolutionMethod::EXACT && payMethod == PaymentMethod::MULTIPATH ) {
            pd = new PaymentDeployerMultipathExact(net->getNumNodes(), amount, src, dst);
        } else if (resMethod==ResolutionMethod::EXACT &&  payMethod == PaymentMethod::SINGLEPATH ) {
            pd = new SinglePathSolver(net->getNumNodes(), amount, src, dst);
        } else if (resMethod==ResolutionMethod::HEURISTIC){
        	if (payMethod == PaymentMethod::SINGLEPATH){
        	        		std::cerr << "Heuristic is not for singlepath mode.\n";
        	        		exit(1);
        	}
        	pd = new MultipathHeuristic(net->getNumNodes(), amount, src, dst, numPaths, hOptimization);
		} else {
        	std::cerr << " The combination of resolution method and method of allocation is not supported.\n";
        	return 1;
        }
        
        /* Add payment channels */
        for ( PaymentChannel * ch: net->getChannels()){

                        /* add directional channel in one direction ... */
                        pd->AddPaymentChannel(ch->getEndPointA()->getId(), ch->getEndPointB()->getId(),
                                    ch->getResidualFundsA(),	ch->getResidualFundsB(),
        							ch->getBaseFee(),
        							ch->getPoints(),
        							ch->getSlopes());

                        /* ... and the other */
                        pd->AddPaymentChannel(ch->getEndPointB()->getId(), ch->getEndPointA()->getId(),
                                    ch->getResidualFundsB(),ch->getResidualFundsA(),
                                    ch->getBaseFee(true),
        							ch->getPoints(true),
        							ch->getSlopes(true));
        }


        if (amount==0) break;

        pd->setAmount(amount);

        std::vector<std::vector<ln_units>> flows(net->getNumNodes(), vector<ln_units
        		>(net->getNumNodes()));
		long fee=0;

		long transferredAmount=0;

		if (pd->RunSolver(flows,fee)==0){
					std::cout << "Success!\n";
					totalFee+=fee;
					net->makePayments(flows);

					int pathLength = stats.calcPathLength(flows);
					stats.averagePathLength = (stats.averagePathLength * stats.successes + pathLength) / (1+stats.successes);
					stats.averageFee = (stats.averageFee * stats.successes + fee) /(1+stats.successes);

					stats.successes++;
					stats.feesPaid+=fee;

					printSolution(cout, string("Solution given by ") + typeid(*pd).name(), flows, fee);

        } else {
					std::cerr<<"Unsuccess!\n";
					stats.fails+=1;
        }


            
/*
		case ResolutionMethod::HEURISTIC:
		{
			double transferredAmount=0;

			while (transferredAmount<amount){
				PaymentDeployer pd(net.getNumNodes(), amount, src, dst);


				for (const PaymentChannel * ch: net.getChannels()){
					double sendingFeeA, receivingFeeA, sendingFeeB, receivingFeeB;
					ch->calcLinearizedFee(amount, sendingFeeA, receivingFeeA, false);
					ch->calcLinearizedFee(amount, sendingFeeB, receivingFeeB, true);


					//if (src<dst){
						if (ch->getEndPointA()->getId()==src) sendingFeeA=0;
						if (ch->getEndPointB()->getId()==dst) receivingFeeB=0;
					//} else {
						if (ch->getEndPointB()->getId()==src) sendingFeeB=0;
						if (ch->getEndPointA()->getId()==dst) receivingFeeA=0;
					//}


					pd.AddPaymentChannel(ch->getEndPointA()->getId(), ch->getEndPointB()->getId(), ch->getResidualFundsA(),
					ch->getResidualFundsB(), sendingFeeA, sendingFeeB, receivingFeeA, receivingFeeB);
				}

				double imb=findSmallestImbalance(net.getChannels());
				pd.setAmount(imb);

				std::vector<std::vector<double>> flows(net.getNumNodes(), vector<double>(net.getNumNodes()));
				double fee=0;

				if (pd.RunSolver(flows,fee)==0){
					std::cout << "Success!\n";
					totalFee+=fee;
					net.makePayments(flows);
					stats.feesPaid+=totalFee;
					transferredAmount+=imb;
				} else {
					std::cerr<<"Unsuccess!\n";
					stats.unsuccess+=1;
					break;
				}

				if (transferredAmount == amount)
					stats.success+=1;
			}

			break;
		}

		case FeeType::PROPORTIONAL:{

			PaymentDeployer pd(net.getNumNodes(), amount, src, dst);

			for (const PaymentChannel * ch: net.getChannels()){
								double sendingFeeA, receivingFeeA, sendingFeeB, receivingFeeB;
								ch->calcLinearizedFee(amount, sendingFeeA, receivingFeeA, false);
								ch->calcLinearizedFee(amount, sendingFeeB, receivingFeeB, true);

								//if (src<dst){
													if (ch->getEndPointA()->getId()==src) sendingFeeA=0;
													if (ch->getEndPointB()->getId()==dst) receivingFeeB=0;
								//				} else {
													if (ch->getEndPointB()->getId()==src) sendingFeeB=0;
													if (ch->getEndPointA()->getId()==dst) receivingFeeA=0;
								//}


								pd.AddPaymentChannel(ch->getEndPointA()->getId(), ch->getEndPointB()->getId(),
										ch->getResidualFundsA(), ch->getResidualFundsB(), sendingFeeA, sendingFeeB,
										receivingFeeA, receivingFeeB);
			}

			std::vector<std::vector<double>> flows(net.getNumNodes(), vector<double>(net.getNumNodes()));


			if (pd.RunSolver(flows,totalFee)==0){
					std::cout << "Success!\n";
					net.makePayments(flows);
					stats.success+=1;
					stats.feesPaid+=totalFee;
					checkPayment(flows, amount, totalFee, src, dst);

			} else {
					stats.unsuccess+=1;
					std::cerr<<"Unsuccess!\n";
					break;
			}

			break;
		}


		case FeeType::GENERIC:{
				PaymentDeployer pd(net.getNumNodes(), amount, src, dst);
				break;

			}
		}
*/ 
        delete pd;

	} while (time<totalTime && stats.numPayments < maxPayments );

	cout.precision(8);
	stats.print();
	std::cout << "Total funds at the beginning: " << initFunds << "\n";
	std::cout << "Total funds at the end: " << net->totalFunds() << "\n";

	delete net;
	delete paymGen;

	assert(stats.successes+stats.fails==stats.numPayments);

	return 0;
}

//#endif
