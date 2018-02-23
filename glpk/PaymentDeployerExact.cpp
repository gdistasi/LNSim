/*
 * PaymentDeployer.cpp
 *
 *  Created on: 07 lug 2016
 *      Author: giovanni
 */

#include "PaymentDeployerExact.h"


#include <stdlib.h>
#include <math.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>

#include <string.h>

#include <assert.h>

#include<iostream>
#include<string>
#include <fstream>

#include "../utils.h"

#include <regex>

#include "../defs.h"


using namespace std;


std::vector<unsigned long> PaymentDeployerExact::getCoefficients(int x, int y){
	std::pair<int,int> p = std::pair<int,int>(x,y);
	std::vector<unsigned long> empty;

	if (fees.find(p)!=fees.end()){
		return fees[p].coefficients;
	} else {
		//std::cout << "RETURNING EMPTY\m";
		return empty;
	}
}

std::vector<unsigned long> PaymentDeployerExact::getPoints(int x, int y){
	std::pair<int,int> p = std::pair<int,int>(x,y);
	std::vector<unsigned long> empty;

	if (fees.find(p)!=fees.end()){
		return fees[p].starting_points;
	} else {
		return empty;
	}

}

unsigned long PaymentDeployerExact::getBaseFee(int x, int y){
	std::pair<int,int> p = std::pair<int,int>(x,y);

	if (fees.find(p)!=fees.end()){
		return fees[p].baseFee;
	} else {
		return 0;
	}

}

void PaymentDeployerExact::AddPaymentChannel(int A, int B, unsigned long resFundsA, unsigned long resFundsB, unsigned long baseFee, std::vector<unsigned long> sp, std::vector<unsigned long> cfs){

	fees.insert( std::pair< std::pair<int,int> , PiecewiseLinearFee > ( std::pair<int, int>(A,B), PiecewiseLinearFee(baseFee, sp, cfs) ) );
	std::cout << "AAA--Adding payment channel between " << A << " and " << B << " with funds " << resFundsA << " and " << resFundsB << "\n";
	channels.insert( std::pair< std::pair<int,int> , PaymentChannel > ( std::pair<int, int>(A,B), PaymentChannel(A,B, resFundsA, resFundsB) ));

}



int  PaymentDeployerExact::RunSolver( std::vector< std::vector<long> > & flow, long & totalFee) {

		int i, j;
		char commandString[1024], solStatus[1024];
		int nodeNum=numNodes;

		/* create temporary files */
		char * dataFile = tempnam(0, "DATA");
		char * outputFile = tempnam(0, "OUT");
		//char * clpModel = tempnam(0, "CLP");

		std::cout << "Temp file " << outputFile << "\n";
		std::cout << "Data file " << dataFile << "\n";



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
				fprintf(fpDat, "%lu ", resFunds(i,j));
			}
			fprintf(fpDat, "\n");
		}
		fprintf(fpDat, ";\n\n");

		// base fees
		fprintf(fpDat, "param baseSendingFee:\n");
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
		}
		fprintf(fpDat, ":=\n");

				// write down the r matrix
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
			for (j = 0; j < nodeNum; j++) {
				fprintf(fpDat, "%lu ", getBaseFee(i,j));
			}
			fprintf(fpDat, "\n");
		}
		fprintf(fpDat, ";\n\n");

		fprintf(fpDat, "param sendingFeeNumRegions:\n");
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
		}
		fprintf(fpDat, ":=\n");

		// write down the r matrix
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
			for (j = 0; j < nodeNum; j++) {
				fprintf(fpDat, "%d ", getCoefficients(i,j).size() );
			}
			fprintf(fpDat, "\n");
		}
		fprintf(fpDat, ";\n\n");

		// residual funds constraints - not really used for solving, just for debugging
		/*fprintf(fpDat, "param sendingFeeRates :=\n");
		for (i = 0; i < nodeNum; i++) {
			for (j = 0; j < nodeNum; j++) {
				if (resFunds(i,j)== 0 ) continue;

				fprintf(fpDat, "[%d,%d,*] ", i,j);
				for (int p=1; p <= getCoefficients(i,j).size(); p++){
					fprintf(fpDat, " %lu %lu  ", p, getCoefficients(i,j)[p-1]);

					std::cout << getCoefficients(i,j)[p-1];

				}
			}
		}*/


		// Sending fee value at the limit points (only the piecewise part, i.e. y(0)=0)
		fprintf(fpDat, "\n\nparam yy :=\n");
				for (i = 0; i < nodeNum; i++) {
					for (j = 0; j < nodeNum; j++) {
						if (resFunds(i,j)== 0 ) continue;

						fprintf(fpDat, "[%d,%d,*] ", i,j);
						fprintf(fpDat, " %lu %lu  ", 1, 0);

						long fee=0;

						for (int p=2; p <= getPoints(i,j).size(); p++){

							/* getCoefficients specify the amount of millisatoshis to pay for each satoshi transferred */
							std::cout << "\nPp-1 " << getPoints(i,j)[p-1] << " Pp-2 " << getPoints(i,j)[p-2] << " Coeff-p " << getPoints(i,j)[p-2] << "\n";

							fee = ((getPoints(i,j)[p-1] - getPoints(i,j)[p-2])) * getCoefficients(i,j)[p-2] / 1000;

							std::cout << "fee " << fee << "\n";

							fprintf(fpDat, " %lu %lu  ", p, fee);

							std::cout << getCoefficients(i,j)[p-1];

						}
					}
				}


		fprintf(fpDat, " ;\n\n");

		// residual funds constraints
		fprintf(fpDat, "param sendingFeeLimits :=\n");
		for (i = 0; i < nodeNum; i++) {
			for (j = 0; j < nodeNum; j++) {
				if (resFunds(i,j)==0 ) continue;

				fprintf(fpDat, "[%d,%d,*] ", i,j);
				for (int p=0; p < getPoints(i,j).size(); p++){
					fprintf(fpDat, " %lu %lu ", p, getPoints(i,j)[p]);
				}
			}
		}

		fprintf(fpDat, " ;\n");


	/*	// residual funds constraints
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
*/

		fprintf(fpDat, "\nparam source := %d;\n\n", source);
		fprintf(fpDat, "param destination := %d;\n\n", destination);
		fprintf(fpDat, "param P := %d;\n\n", payment);

		fprintf(fpDat, "end;\n\n");

		fclose(fpDat);

		sprintf(commandString, "glpsol --model %s --data %s -o %s",
						(modelsDirectory+string("/ModelExact")).c_str(), dataFile, outputFile);
		cout << commandString << endl;
	//	::system(commandString);

	//	sprintf(commandString, "glpsol --model %s --data %s -w %s",
		//						"/home/giovanni/windows/workspace/PaymentChannelsSimulator/glpk/Model", dataFile, outputFile);
		//::system(commandString);


		string res=exec(commandString);

		std::cout << res;

		std::size_t found=res.find("SOLUTION FOUND");
		if (found==std::string::npos){
				return PAYMENT_FAILED;
		}

		cout << "Opening output file...\n";
		// open output file

		ifstream fpOut (outputFile);

		if (fpOut.is_open() == false) {
			cout << "failed to open file: " << outputFile << endl;
			return COULD_NOT_OPEN_OUTPUT_FILE;
		}

		string line;

		int rows;
		int columns;


	    getline (fpOut,line);
	    getline (fpOut,line);

		rows=convertTo(tokenize(line)[1]);

	    getline (fpOut,line);

		columns=convertTo(tokenize(line)[1]);

	    //cout << "COLUMNS " << columns << "\n";
		//cout << "RW " << rows << "\n";

		//assert(columns==numNodes*numNodes);

		//fscanf(fpOut, "%s", solStatus);
		//fscanf(fpOut, "%s", solStatus);
		//fscanf(fpOut, "%s", solStatus);

		getline (fpOut,line);

		getline (fpOut,line);

		getline (fpOut,line);

		totalFee = convertTo(tokenize(line)[3]) - payment;

		std::cout<<"Total fees " << totalFee << "\n";

		//go to the solutions
		found=line.find("Column name");
		while (found==std::string::npos){
			getline (fpOut,line);
			found=line.find("Column name");
		}
		getline (fpOut,line);


		long fVal;

		for (int i=0; i<numNodes; i++)
			for (int j=0; j<numNodes; j++){

				getline(fpOut, line);

				//std::cout << "LINE WITH SOLUTIONS " << line << "\n";

				fVal=convertTo(tokenize(line)[3]);

				//if (fabs(fVal)>EPSILON){

					flow[i][j]=fVal;
					fprintf(stdout, "flow[%d,%d]=%lu\n",i,j, flow[i][j]);
					//std::cout << "flow["<<i<<","<<j<<"]="<<flow[i][j]<<  "\n";

				//} else {
				////	flow[i][j]=0;
				//}
				//fscanf(fpOut, "%s", solStatus);

		}

		fpOut.close();
		cout << "Finished parsing.\n";

		return 0;
}







