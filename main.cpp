
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
#include <limits>
#include <typeinfo>

#include "SinglePathSolver.h"
#include "MultipathHeuristic.h"

#include <random>

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


void printHelp(){

	std::cout << "Please refer to the README file included in the distribution for help on how to use this tool." << "\n";

}

int main(int argc, char * * argv){

	int seed=0;
	ln_units meanSizePayments=1000000;
	double variancePayments=500000;
	double intervalPayments=1;
	double totalTime=std::numeric_limits<double>::max();
	int numNodes=0;
	int numSources=1;
	int numDestinations=1;
	double connectionProbability=0.1;
    
    // Interval between fees' recalculation.
    double tbfu=0;

	ln_units minFund=0.01 * MILLISATOSHIS_IN_BTC;
	ln_units maxFund=0.167 * MILLISATOSHIS_IN_BTC;

	//double sendingFee=1000 ;
	//double receivingFee=0;
	//double positiveSlope=sendingFee;
	//double negativeSlope=sendingFee*0.01;


	/* in millisatoshis */
	long basefee=1000;

	/* in millisatoshis per kw (kiloweight - 1000 satoshis) transferred */
	long feerate_low=10;
	long feerate_high=30;

	/* in satoshis */
	//ln_units baseFee=1;


	int maxPayments=std::numeric_limits<int>::max();
	string networkFile="";
	string paymentsFile="";

	bool createPaymentsFile=false;
	bool createNetworkFile=false;
	string paymentsFileToCreate;
	string networkFileToCreate;



	int averageNumSlopes=3;

	int numPaths=3;
    
    ResolutionMethod resMethod=ResolutionMethod::EXACT;
    FeeType feePolicy=FeeType::GENERAL;
    PaymentMethod payMethod=PaymentMethod::SINGLEPATH;

    string logFile="log";
    ofstream logFileO;

    bool hOptimization=true;

	//handling of options
	while (1)
	    {
	      static struct option long_options[] =
	        {
 	          {"help",     no_argument,       0, 'h'},
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
			  {"networkFile", required_argument, 0, 'N'},
			  {"paymentsFile", required_argument, 0, 'Q'},
			  {"baseFee", required_argument, 0, 'B'},
			  {"feerateLow", required_argument, 0, '1'},
			  {"feerateHigh", required_argument, 0, '2'},
			  {"heuristicOptimization", required_argument, 0, 'O'},
			  {"logFile", required_argument, 0, 'L'},
			  {"makePaymentsFile", required_argument, 0, 'a'},
			  {"makeNetworkFile", required_argument, 0, 'A'},
  			  {"tbfu", required_argument, 0, 'F'},
			  {"seed",   required_argument, 0, 'S'},

	          {0, 0, 0, 0}
	        };
	      /* getopt_long stores the option index here. */
	      int option_index = 0;

	      int c = getopt_long (argc, argv, "s:v:i:t:n:p:m:M:r:f:P:F:h",
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

	        case 'A':
	   	        	networkFileToCreate=string(optarg);
	   	        	createNetworkFile=true;
	   	        	break;
	        case 'a':
	        		paymentsFileToCreate=string(optarg);
	   	        	createPaymentsFile=true;
	   	        	break;


	        case 's':
	        	meanSizePayments=atof(optarg);
	        	break;

	        case 'B':
	       	  	basefee=atoi(optarg);
	       	  	break;


	        case 'P':
	  	        	maxPayments=atoi(optarg);
	  	        	break;

	        case 'N':
	    	  	    networkFile=string(optarg);
	    	  	    break;

	        case 'L':
                    logFile=string(optarg);
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

	        case '1':
	 	        	feerate_low=atoi(optarg);
	 	        	break;
	        case '2':
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
                
            case 'F':
	        	tbfu=atof(optarg);
	        	break;

	        case 'd':
	        	modelsDirectory=string(optarg);
	        	break;
	        case 'S':
	        	seed=atoi(optarg);
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

	        case 'h':
	        	printHelp();
	        	exit(0);

	        default:
	          abort ();
	        }
	    }


	if (numNodes == 0 && networkFile == ""){
		std::cerr << "No topology specified.\n";
		printHelp();
		exit(1);
	}




	LightningNetwork * net=0;

	std::cerr << "All values are expressed in millisatoshis when the unit is not specified.\n";

	if (networkFile!=""){
		std::cerr << "Reading network from file...\n";
		net = NetworkGenerator::generateBaseFromFile(networkFile);
	} else {
		std::cerr << "Generating network...\n";
		net = NetworkGenerator::generateBase(numNodes, connectionProbability, minFund, maxFund, seed);
	}

	if (feePolicy = GENERAL_OPTIMIZED){
		net=NetworkGenerator::generateOptimizedFee(net, basefee ,
													  feerate_high , feerate_low, seed);
	} else {
//		net = new NetworkGenerator::generate(numNodes, connectionProbability, minFund, maxFund, sendingFee,
//		receivingFee, positiveSlope, negativeSlope, feePolicy, seed, averageNumSlopes);
		std::cerr << "Not supported! " << endl;
		exit(1);
	}

	std::cerr << "Done.\n";


	set<int> toAvoid;

	if (!net->connected()){

		std::cerr << "Graph is not connected!\n";
		return 1;

		for (int i=0; i<net->vis.size(); i++){
			if (!net->vis[i]){
				std::cerr << "Node " << i << " not connected.\n";
				toAvoid.insert(i);
			}
		}

		//exit(1);
	}

	std::cerr << "Topology: \n";
	net->dumpTopology(cout);

	std::cerr << "Initial funds: " << net->totalFunds() << std::endl;

	PaymentsGenerator * paymGen=0;

	if (paymentsFile!=""){
		paymGen = new PaymentGeneratorFromFile(paymentsFile);
	} else {
		paymGen = new NormalSizePoissonTimePaymentGenerator(net->getNumNodes(), numSources, numDestinations,
										  seed, meanSizePayments, variancePayments, intervalPayments, toAvoid);
	}



	double time;
	ln_units amount;
	int src,dst;
    
	long initFunds = net->totalFunds();

	Stats stats;


	if (createPaymentsFile){
			int numPayments=0;
			ofstream file;
			file.open(paymentsFileToCreate);
			while (numPayments<=maxPayments){
				paymGen->getNext(amount,time,src,dst);
				file << time << " " << amount << " " << src << " " << dst << "\n";
				numPayments++;
			}
			file.close();
			exit(0);
	}

	if (createNetworkFile){
		ofstream file;
		file.open(networkFileToCreate);
		net->dumpTopology(file);
		file.close();
		exit(0);
	}

	logFileO.open(logFile);

	logFileO << "time amount src dst successes fails "
				 << "acceptRatio averageImbalance averagePathLength averageFee feesPaid minFunds maxFunds\n";


                 
    bool firstPayment=true;
    
    
    
    if (tbfu>0){
        
       std::default_random_engine generator;
       std::normal_distribution<double> distribution(tbfu,tbfu/4);
       
       double t;
       for ( PaymentChannel * ch: net->getChannels()){
            
            t=distribution(generator);
            
            if (t>0)
                ch->setTbfu(t);
       }

    }
    
                 
	do {

		net->checkResidualFunds();

		long fee=0;
		long transferredAmount=0;
		bool success=false;
		ln_units totalFee=0;
        
		std::vector<std::vector<ln_units>> flows(net->getNumNodes(), vector<ln_units
        		>(net->getNumNodes()));

		std::cerr << "Getting next payment info...";
		paymGen->getNext(amount,time,src,dst);

        
        for ( PaymentChannel * ch: net->getChannels()){
                if (time - ch->getLastTimeFeeUpdate() > ch->getTbfu()){
                        ch->updateFees(time);
                }
        }
        

		std::cerr << "Processing next payment - amount: " << ((double)amount)/MILLISATOSHIS_IN_BTC << "btc time: " << time << "s Src: " << src << " Dst: " << dst << "\n";

		if (!net->hasEnoughFunds(amount,src,payMethod)){
			std::cerr << "Source has not enough funds\n";
			stats.notEnoughFundsSource++;
			goto nextpay;
		}

		stats.numPayments+=1;


        PaymentDeployer * pd;
        
        if (resMethod==ResolutionMethod::EXACT && payMethod == PaymentMethod::MULTIPATH ) {
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


       // if (amount==0) break;

        pd->setAmount(amount);



		if (pd->RunSolver(flows,fee)==0){
            
					std::cout << "Success!\n";
					totalFee+=fee;
					net->makePayments(flows);

					int pathLength = stats.calcPathLength(flows);
					stats.averagePathLength = (stats.averagePathLength * stats.successes + pathLength) / (1+stats.successes);
					stats.averageFee = (stats.averageFee * stats.successes + fee) /(1+stats.successes);

					stats.successes++;
					stats.feesPaid+=fee;

					success=true;

					printSolution(cout, string("Solution given by ") + typeid(*pd).name(), flows, fee);

        } else {
            
					std::cerr<<"Unsuccess!\n";
					stats.fails+=1;
                    
        }

        delete pd;

nextpay:

		double acceptRatio = ((double)stats.successes) / stats.numPayments;

		std::cerr << "Success ratio: " << acceptRatio << "\n";
		logFileO << time << " " << amount << " "<< src << " " << dst << " " << stats.successes << " " << stats.fails
				<< " " << acceptRatio << " " << net->averageImbalance() << " " << stats.averagePathLength << " " << stats.averageFee << " " << stats.feesPaid
				<< " " << net->minFunds() << " " << net->maxFunds() <<   "\n";

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

	} while (time<totalTime && stats.numPayments < maxPayments );

	cout.precision(8);
	stats.print();
	std::cout << "Total funds at the beginning: " << initFunds << "\n";
	std::cout << "Total funds at the end: " << net->totalFunds() << "\n";

	delete net;
	delete paymGen;

	assert(stats.successes+stats.fails==stats.numPayments);

	logFileO.close();

	return 0;
}

//#endif
