
#include "LightningNetwork.h"
//#ifdef MAIN
#include "PaymentsGenerator.h"
#include "glpk/PaymentDeployer.h"
#include "NetworkGenerator.h"

#include <cmath>
#include <iostream>
#include "defs.h"
#include <string.h>
#include <stdio.h>

#include "glpk/PaymentDeployerExact.h"

#include <getopt.h>

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

void checkPayment(std::vector<std::vector<double>> flows, double pay, double fee, int src, int dst){

	std::cout << "Checking payment...\n";

	double out=0,in=0;

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

class Stats {


public:
	Stats(){ success=0; unsuccess=0; total=0; payments=0; feesPaid=0; }

	void print(){
		std::cout << "Payments: " << payments << "\n";
		std::cout << "Successes: " << success << "\n";
		std::cout << "Unsuccesses: " << unsuccess << "\n";
		std::cout << "Fees paid: " << feesPaid << "\n";

	}

	int success;
	int unsuccess;
	int payments;
	double feesPaid;
	int total;
};

int main(int argc, char * * argv){

	int seed=0.1;
	double meanSizePayments=0.001;
	double variancePayments=0.1;
	double intervalPayments=1;
	double totalTime=60; //one day
	double numNodes=2;
	int numSources=1;
	int numDestinations=1;
	double connectionProbability=1;
	double minFund=0.01;
	double maxFund=0.167;
	double sendingFee=0.00000001;
	double receivingFee=0;
	double positiveSlope=sendingFee;
	double negativeSlope=sendingFee*0.01;
	double slow=0.00000001;
	double shigh=0.00000002;
	double baseFee=0.00000001;

	int averageNumSlopes=3;
    
    ResolutionMethod resMethod=ResolutionMethod::EXACT;
    FeeType feePolicy=FeeType::GENERAL;
    PaymentMethod payMethod=PaymentMethod::SINGLEPATH;

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
			  {"connectionProbability",    required_argument, 0, 'P'},
	          {"minFundsChannels",    required_argument, 0, 'm'},
			  {"maxFundsChannels",    required_argument, 0, 'M'},
			  {"modelsDirectory",   required_argument, 0, 'd'},
              {"feePolicy", required_argument, 0, 'f'},
			  {"resolutionMethod", required_argument, 0, 'r'},
			  {"paymentMethod", required_argument, 0, 'p'},
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

	        case 'v':
	        	variancePayments=atof(optarg);
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

	        case 'P':
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
                } else if (strcmp("multipath", optarg)==0){
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

	Stats stats;
	LightningNetwork * net=0;

	std::cerr << "Generating network...\n";


	if (feePolicy = GENERAL_OPTIMIZED){
		net = NetworkGenerator::generateOptimized(numNodes, connectionProbability, minFund * SATOSHI_IN_BTC, maxFund * SATOSHI_IN_BTC, sendingFee * SATOSHI_IN_BTC,
													  shigh * SATOSHI_IN_BTC, slow * SATOSHI_IN_BTC, seed);
	} else {
//		net = new NetworkGenerator::generate(numNodes, connectionProbability, minFund, maxFund, sendingFee,
//		receivingFee, positiveSlope, negativeSlope, feePolicy, seed, averageNumSlopes);
		std::cerr << "Not supported! " << endl;
		exit(1);
	}

	std::cerr << "Done.\n";

	std::cerr << "Initial funds: " << net->totalFunds() << std::endl;

	NormalSizePoissonTimePaymentGenerator paymGen(net->getNumNodes(), numSources, numDestinations,
										  seed, meanSizePayments * SATOSHI_IN_BTC, variancePayments, intervalPayments);

	double time;
	double amount;
	int src,dst;

	double initFunds = net->totalFunds();


	do {

		net->checkResidualFunds();

		std::cerr << "Generating next payment...";
		paymGen.getNext(amount,time,src,dst);

		stats.payments+=1;
		std::cerr << "Processing next payment - amount: " << amount << " time: " << time << " Src: " << src << " Dst: " << dst << "\n";

		double totalFee=0;

        PaymentDeployer * pd;
        
        if (    resMethod==ResolutionMethod::EXACT &&
                ( payMethod == PaymentMethod::SINGLEPATH || payMethod == PaymentMethod::MULTIPATH )) {
            
            
            std::vector<std::vector<double>> flows(net->getNumNodes(), vector<double>(net->getNumNodes()));
			double fee=0;
            
            pd = new PaymentDeployerExact(net->getNumNodes(), amount, src, dst);

			for ( PaymentChannel * ch: net->getChannels()){

                /* add directional channel in one direction ... */
                (dynamic_cast<PaymentDeployerExact *>(pd))->AddPaymentChannel(ch->getEndPointA()->getId(), ch->getEndPointB()->getId(),
                            ch->getResidualFundsA(),	ch->getResidualFundsB(), 
							ch->getBaseFee(),
							ch->getPoints(),
							ch->getSlopes());
                
                /* ... and the other */
                (dynamic_cast<PaymentDeployerExact *>(pd))->AddPaymentChannel(ch->getEndPointB()->getId(), ch->getEndPointA()->getId(),
                            ch->getResidualFundsB(),ch->getResidualFundsA(), 
                            ch->getBaseFee(true),
							ch->getPoints(true),
							ch->getSlopes(true));
            }
        
                    
        }
        
        pd->setAmount(amount);

        std::vector<std::vector<double>> flows(net->getNumNodes(), vector<double>(net->getNumNodes()));
		double fee=0;

		long transferredAmount=0;

		std::cout << "QUI\n";

		if (pd->RunSolver(flows,fee)==0){
					std::cout << "Success!\n";
					totalFee+=fee;
					net->makePayments(flows);
					stats.feesPaid+=totalFee;
					transferredAmount+=amount;
        } else {
					std::cerr<<"Unsuccess!\n";
					stats.unsuccess+=1;
                    break;
        }


        if (transferredAmount == amount)
			stats.success+=1;
        else
            stats.unsuccess+=1;
            
            
            
            
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

	} while (time<totalTime);

	cout.precision(8);
	stats.print();
	std::cout << "Total funds at the beginning: " << initFunds << "\n";
	std::cout << "Total funds at the end: " << net->totalFunds() << "\n";


	return 0;
}

//#endif
