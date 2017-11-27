
#include "LightningNetwork.h"
//#ifdef MAIN
#include "PaymentsGenerator.h"
#include "glpk/PaymentDeployer.h"
#include "NetworkGenerator.h"

#include <cmath>
#include <iostream>
#include "defs.h"

#include <getopt.h>

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
	double meanSizePayments=1;
	double variancePayments=0.1;
	double intervalPayments=1;
	double totalTime=60; //one day
	double numNodes=10;
	int numSources=3;
	int numDestinations=3;
	double connectionProbability=0.45;
	double minFund=0.1;
	double maxFund=10;
	double sendingFee=0.000001;
	double receivingFee=0;
	double positiveSlope=sendingFee;
	double negativeSlope=sendingFee*0.01;
	int averageNumSlopes=3;

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
			  {"connectionProbability",    required_argument, 0, 'p'},
	          {"minFundsChannels",    required_argument, 0, 'm'},
			  {"maxFundsChannels",    required_argument, 0, 'M'},
			  {"modelsDirectory",   required_argument, 0, 'd'},
			  {"seed",   required_argument, 0, 'S'},

	          {0, 0, 0, 0}
	        };
	      /* getopt_long stores the option index here. */
	      int option_index = 0;

	      int c = getopt_long (argc, argv, "s:v:i:t:n:p:m:M:",
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

	        case 'p':
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
	        case '?':
	          /* getopt_long already printed an error message. */
	        	exit(1);
	          break;

	        default:
	          abort ();
	        }
	    }



	Stats stats;

	FeeType feeType=PROPORTIONAL;

	LightningNetwork net = NetworkGenerator::generate(numNodes, connectionProbability, minFund, maxFund, sendingFee,
													receivingFee, positiveSlope, negativeSlope, feeType, seed, averageNumSlopes);

	NormalSizePoissonTimePaymentGenerator paymGen(net.getNumNodes(), numSources, numDestinations,
										  seed, meanSizePayments, variancePayments, intervalPayments);

	double time;
	double amount;
	int src,dst;

	double initFunds=net.totalFunds();


	do {

		net.checkResidualFunds();

		paymGen.getNext(amount,time,src,dst);

		stats.payments+=1;
		std::cout << "Processing next payment - amount: " << amount << " time: " << time << " Src: " << src << " Dst: " << dst << "\n";


		double totalFee=0;

		switch(net.getFeePolicy()){

		case FeeType::FIXED:
			//NON FATTIBILE PER IL MOM
			break;

		case FeeType::BILANCING:
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



	} while (time<totalTime);

	cout.precision(8);
	stats.print();
	std::cout << "Total funds at the beginning: " << initFunds << "\n";
	std::cout << "Total funds at the end: " << net.totalFunds() << "\n";


	return 0;
}

//#endif
