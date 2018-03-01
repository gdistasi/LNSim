/*
 * PaymentDeployer.cpp
 *
 *  Created on: 07 lug 2016
 *      Author: giovanni
 */

#include "PaymentDeployer.h"
#include <stdlib.h>
#include <math.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <cassert>
#include <cmath>
#include <algorithm>

#include <string.h>
#include <cstring>

#define DBL_MAX std::numeric_limits<double>::max()


using namespace std;

PaymentDeployer::PaymentDeployer(const PaymentDeployer & pd){
	unsigned char * src = (unsigned char *)&pd;
	unsigned char * dst = (unsigned char *)this;
	memcpy(dst, src, sizeof(*this));
}

PaymentDeployer::~PaymentDeployer(){
	for (auto ch: channels){
		delete ch.second;
	}
}

long PaymentDeployer::resFunds(int x,int y){
	//std::cout << "FINDING " << channels.find( pair<int,int>(x,y))->second.resFundsA  ;

	if (channels.find( pair<int,int>(x,y)) == channels.end()) return 0;
	//std::cout << "ResFunds " << x << " y " << y << " " << channels.find( pair<int,int>(x,y) )->second.resFundsA << "\n";
	else return channels.find( pair<int,int>(x,y) )->second->resFundsA;
}

map<pair<int, int>, PaymentDeployer::PaymentChannel *> & PaymentDeployer::getChannels(){ return channels;}

/*
int  PaymentDeployer::RunSolverOld(std::vector<std::vector<double>> & flow, double & totalFee) {

		int i, jd;
		char commandString[1024], solStatus[1024];

		int nodeNum=numNodes;

		/* create temporary files
		char * dataFile = tempnam(0, "DATA");
		char * outputFile = tempnam(0, "OUT");
		//char * clpModel = tempnam(0, "CLP");

		// open data file
		FILE *fpDat = fopen(dataFile, "w");
		if (fpDat == NULL) {
			cout << "failed to open file: " << dataFile << endl;
			return COULD_NOT_OPEN_DATA_FILE;
		}

		// create data file
		fprintf(fpDat, "\ndata;\n\n");

		// gateway set
		fprintf(fpDat, "set N:=");
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, " %d", i);
		}
		fprintf(fpDat, ";\n\n");


		// residual funds constraints
		fprintf(fpDat, "param r:\n");
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
		}
		fprintf(fpDat, ":=\n");

		// write down the r matrix
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
			for (j = 0; j < nodeNum; j++) {
				fprintf(fpDat, "%.8lf ", resFunds(i,j));
			}
			fprintf(fpDat, "\n");
		}
		fprintf(fpDat, ";\n\n");


		// residual funds constraints
		fprintf(fpDat, "param sendingFee:\n");
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
		}
		fprintf(fpDat, ":=\n");

		// write down the r matrix
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
			for (j = 0; j < nodeNum; j++) {
				fprintf(fpDat, "%.8lf ", sendingFee(i,j));
			}
			fprintf(fpDat, "\n");
		}
		fprintf(fpDat, ";\n\n");


		// residual funds constraints
			fprintf(fpDat, "param receivingFee:\n");
			for (i = 0; i < nodeNum; i++) {
				fprintf(fpDat, "%d ", i);
			}
			fprintf(fpDat, ":=\n");

			// write down the r matrix
			for (i = 0; i < nodeNum; i++) {
				fprintf(fpDat, "%d ", i);
				for (j = 0; j < nodeNum; j++) {
					fprintf(fpDat, "%.8lf ", receivingFee(i,j));
				}
				fprintf(fpDat, "\n");
			}
			fprintf(fpDat, ";\n\n");



		fprintf(fpDat, "param source := %d;\n\n", source);
		fprintf(fpDat, "param destination := %d;\n\n", destination);
		fprintf(fpDat, "param P := %.8lf;\n\n", payment);

		fprintf(fpDat, "end;\n\n");

		fclose(fpDat);


		sprintf(commandString, "glpsol --model %s --data %s --output %s",
						(modelsDirectory+"/Model").c_str(), dataFile, outputFile);
		cout << commandString << endl;



		cout << "Opening output file...\n";
		// open output file

		FILE *fpOut = fopen(outputFile, "r");
		if (fpOut == NULL) {
			cout << "failed to open file: " << outputFile << endl;
			return COULD_NOT_OPEN_OUTPUT_FILE;
		}

		// parse result from the output file
		for (i = 0; i < 10; i++) {
				fscanf(fpOut, "%s", solStatus);
		}

		if (strcmp(solStatus, "OPTIMAL") != 0) {
				fclose(fpOut);

				return PAYMENT_FAILED;
		}

		for (i = 0; i < 4; i++) {
				fscanf(fpOut, "%s", solStatus);
		}

		// LP minimized value
		totalFee = atof(solStatus);

		cout << "Tot. fees:  " << totalFee << "\n";

		// find the first occurrence of "Marginal"
		while (fscanf(fpOut, "%s", solStatus) == 1) {
			if (strcmp(solStatus, "Marginal") == 0){
				//cout << "OK 1\n";
					break;

			}
		}

		// find the second occurrence of "Marginal"
		// that means we are just on top of flow values
		while (fscanf(fpOut, "%s", solStatus) == 1) {
				if (strcmp(solStatus, "Marginal") == 0){
					//cout << "OK 2\n";
					break;

				}
		}

		// find the first 'flow'
		while (fscanf(fpOut, "%s", solStatus) == 1) {
				if (solStatus[0]=='f'){
					//cout << "OK3\n";
					break;

				}
		}

		// now find out the values
		int x, y, brk;
		double fVal = 0;

		int numF=0;

		while (1) {


				sscanf(solStatus, "flow[%d,%d]", &x, &y);

				fscanf(fpOut, "%s", solStatus);
				fscanf(fpOut, "%s", solStatus);

				fVal = atof(solStatus);

				if (fVal > EPSILON) {
					printf("%d %d %.8lf\n", x, y, fVal);
				}

				//cout << "Setting " << x << ":" << y << "to " << fVal;

				flow[x][y] = fVal;


				numF++;

				if (numF==numNodes*numNodes) break;

				while ((brk = fscanf(fpOut, "%s", solStatus)) == 1) {
						if (solStatus[0]=='f'){
							break;
						}
				}



				//cout << numFlows << "\n";

				//if (numF==numNodes*numNodes-1) break;

		}

		fclose(fpOut);
		cout << "Finished parsing.\n";


		return 0;
}
*/



std::vector<long> PaymentDeployer::getCoefficients(int x, int y){
	std::pair<int,int> p = std::pair<int,int>(x,y);
	std::vector<long> empty;

	if (fees.find(p)!=fees.end()){
		return fees[p].coefficients;
	} else {
		//std::cout << "RETURNING EMPTY\m";
		return empty;
	}
}

std::vector<long> PaymentDeployer::getPoints(int x, int y){
	std::pair<int,int> p = std::pair<int,int>(x,y);
	std::vector<long> empty;

	if (fees.find(p)!=fees.end()){
		return fees[p].starting_points;
	} else {
		return empty;
	}

}

long PaymentDeployer::getBaseFee(int x, int y){
	std::pair<int,int> p = std::pair<int,int>(x,y);

	if (fees.find(p)!=fees.end()){
		return fees[p].baseFee;
	} else {
		return 0;
	}

}

void PaymentDeployer::AddPaymentChannel(int A, int B, long resFundsA, long resFundsB, long baseFee, std::vector<long> sp, std::vector<long> cfs){

	PaymentChannel * pc = new PaymentChannel(A,B, resFundsA, resFundsB);

	fees.insert( std::pair< std::pair<int,int> , PiecewiseLinearFee > ( std::pair<int, int>(A,B), PiecewiseLinearFee(baseFee, sp, cfs) ) );
	//std::cout << "AAA--Adding payment channel between " << A << " and " << B << " with funds " << resFundsA << " and " << resFundsB << "\n";
	channels.insert( std::pair< std::pair<int,int> , PaymentChannel * > ( std::pair<int, int>(A,B),  pc));

	//channelsByNode.insert(std::pair<int, PaymentChannel *>(A, pc));
	channelsByNode[A].push_back(pc);
}

double PaymentDeployer::PiecewiseLinearFee::calcFee(long payment){

	if (payment==0) return 0;

	double fee = baseFee;

	double rest = payment;

	int i=1;

	while (rest>0){

		if ( rest >= (starting_points[i] - starting_points[i-1])){
			fee +=  ( starting_points[i] - starting_points[i-1] ) * coefficients[i-1] / 1000;
			rest -= starting_points[i] - starting_points[i-1];
		} else {
			fee += rest *  coefficients[i-1] / 1000;
			break;
		}
	}

	return fee;

}

/* calc the distance from i to j: the amount of fee required to carry the payment P from i to j */
double PaymentDeployer::calcFee(int i, int j, double payment){

	//vector<PaymentChannel *> chs = channelsByNode[i];
	assert(i!=j);

	double dis=DBL_MAX;

	if (channels.find(std::pair<int,int>(i,j))==channels.end() ||  channels[std::pair<int,int>(i,j)]->resFundsA < round(payment)){
		return dis;
	}

	if (i == source) return 0;

	if (fees.find(pair<int,int>(i, j))!=fees.end()){
		//std::cout << "fee " << fees[pair<int,int>(i, j)].calcFee(payment) << "\n";
		dis = fees[pair<int,int>(i, j)].calcFee(payment);
	}

	return dis;
}



std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

int PaymentDeployer::removeUsedCapacity(Tpath path){
	for (auto f: path){
		//std::cout << "Capa prima " << channels[pair<int,int>(f.first.first,f.first.second)]->resFundsA << "\n";
		assert(channels.find(pair<int,int>(f.first.first,f.first.second))!=channels.end());
		channels[pair<int,int>(f.first.first,f.first.second)]->resFundsA-=f.second;
		//std::cout << "Capa dopo " << channels[pair<int,int>(f.first.first,f.first.second)]->resFundsA << "\n";

	}
}

void convertPathsToFlows(vector<Tpath> paths, Tflows & flows){

	for (int i=0; i<paths.size(); i++){
		for (auto f: paths[i]){
			flows[f.first.first][f.first.second] += f.second;
		}
	}
}

Tpath convertFlowsToPath(Tflows flows, int source, int destination){
	Tpath path;

	int curr=source;

	while (curr!=destination){
		for (int i=0; i<flows.size(); i++){
				if (flows[curr][i]!=0){
					path.push_back(Tpath_el(pair<int,int>(curr,i),flows[curr][i]));
					curr=i;
				}
		}
	}

	return path;
}

#ifdef TESTPAYMENTDEPLOYER


	int main(){
		double res[10][10

					   ];
		double fee=0;


		/*PaymentDeployer pd(5, 0.2, 0, 4);
		pd.AddPaymentChannel(0, 1, 0.5, 0.5, 0.0001, 0.0001);
		pd.AddPaymentChannel(0, 2, 0.5, 0.5, 0.00005, 0.00005);
		pd.AddPaymentChannel(1, 2, 0.5, 0.5, 0.0001, 0.0001);
		pd.AddPaymentChannel(1, 4, 0.5, 0.5, 0.0001, 0.0001);
		pd.AddPaymentChannel(2, 3, 0.5, 0.5, 0.000, 0.000);
		pd.AddPaymentChannel(3, 4, 0.2, 0.8, 0.0001, 0.0001);*/



		PaymentDeployer pd(4, 0.2, 0, 3);
		pd.AddPaymentChannel(0, 1, 0.5, 0.5, 0, 0.0001, 0, 0);
		pd.AddPaymentChannel(0, 2, 0.5, 0.5, 0, 0.0001, 0, 0.0001);
		pd.AddPaymentChannel(2, 3, 0.5, 0.5, 0.0001, 0.0001, 0.0001, 0);
		pd.AddPaymentChannel(1, 3, 0.5, 0.5, 0.0001, 0.0001, 0.0001, 0);
		pd.RunSolver((double **)res,fee);


		PaymentDeployer pd2(5, 0.2, 0, 4);
		pd2.AddPaymentChannel(0, 1, 0.5, 0.5, 0, 0.0001, 0, 0);
		pd2.AddPaymentChannel(0, 2, 0.5, 0.5, 0, 0.0001, 0, 0.0001);
		pd2.AddPaymentChannel(2, 3, 0.5, 0.5, 0.0001, 0.0001, 0.0001, 0);
		pd2.AddPaymentChannel(1, 3, 0.5, 0.5, 0.0001, 0.0001, 0.0001, 0);
		pd2.AddPaymentChannel(3, 4, 0.5, 0.5, 0.0001, 0.0001, 0.0001, 0);


		pd2.RunSolver((double **)res,fee);

	}

#endif






