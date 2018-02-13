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

#include <string.h>

#include <assert.h>


#include "PaymentDeployerExact.h"

using namespace std;


void PaymentDeployerExact::AddPaymentChannel(int A, int B, unsigned long resFundsA, unsigned long resFundsB, unsigned long baseFee, std::vector<unsigned long> sp, std::vector<unsigned long> cfs){
	fees.insert( std::pair< std::pair<int,int> , PiecewiseLinearFee > ( std::pair<int, int>(A,B), PiecewiseLinearFee(baseFee, sp, cfs) ) );
}



int  PaymentDeployerExact::RunSolver( std::vector< std::vector<double> > & flow, double & totalFee) {

		int i, j;
		char commandString[1024], solStatus[1024];
		int nodeNum=numNodes;

		/* create temporary files */
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
				fprintf(fpDat, "%.8lf ", getBaseFee(i,j));
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

		// residual funds constraints
		fprintf(fpDat, "param sendingFeeRates :=\n");
		for (i = 0; i < nodeNum; i++) {
			for (j = 0; j < nodeNum; i++) {
				fprintf(fpDat, "[%d,%d,*] ", i,j);
				for (int p=1; p <= getCoefficients(i,j).size(); p++){
					fprintf(fpDat, " %d %.8lf   ", p, getCoefficients(i,j)[p-1]);
				}
			}
		}


		fprintf(fpDat, " ;\n");

		// residual funds constraints
		fprintf(fpDat, "param sendingFeeLimits :=\n");
		for (i = 0; i < nodeNum; i++) {
			for (j = 0; j < nodeNum; i++) {
				fprintf(fpDat, "[%d,%d,*] ", i,j);
				for (int p=1; p <= getPoints(i,j).size(); p++){
					fprintf(fpDat, " %d %d   ", p, getPoints(i,j)[p-1]);
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

		fprintf(fpDat, "param source := %d;\n\n", source);
		fprintf(fpDat, "param destination := %d;\n\n", destination);
		fprintf(fpDat, "param P := %.8lf;\n\n", payment);

		fprintf(fpDat, "end;\n\n");

		fclose(fpDat);


		sprintf(commandString, "glpsol --model %s --data %s -w %s",
						(modelsDirectory+string("/ModelExact")).c_str(), dataFile, outputFile);
		cout << commandString << endl;
	//	::system(commandString);

	//	sprintf(commandString, "glpsol --model %s --data %s -w %s",
		//						"/home/giovanni/windows/workspace/PaymentChannelsSimulator/glpk/Model", dataFile, outputFile);
		//::system(commandString);


		string res=exec(commandString);

		std::cout << res;

		std::size_t found=res.find("OPTIMAL LP SOLUTION FOUND");
		if (found==std::string::npos){
				return PAYMENT_FAILED;
		}

		cout << "Opening output file...\n";
		// open output file

		FILE *fpOut = fopen(outputFile, "r");
		if (fpOut == NULL) {
			cout << "failed to open file: " << outputFile << endl;
			return COULD_NOT_OPEN_OUTPUT_FILE;
		}

		int rows;
		int columns;

		fscanf(fpOut, "%s", solStatus);
		std::cout << solStatus << "\n";
		rows=atoi(solStatus);

		fscanf(fpOut, "%s", solStatus);
		std::cout << solStatus << "\n";

		columns=atoi(solStatus);

		std::cout << columns << "COL\n";

		assert(columns==numNodes*numNodes);

		fscanf(fpOut, "%s", solStatus);
		fscanf(fpOut, "%s", solStatus);
		fscanf(fpOut, "%s", solStatus);



		// LP minimized value
		totalFee = atof(solStatus);

		std::cout<<"Total fees " << totalFee << "\n";


		for (int i=0; i<rows-1; i++){
			fscanf(fpOut, "%*[^\n]\n", NULL);
		}

		double fVal;

		for (int i=0; i<numNodes; i++)
			for (int j=0; j<numNodes; j++){
				fscanf(fpOut, "%s", solStatus);
				fscanf(fpOut, "%s", solStatus);

				fVal=atof(solStatus);

				if (fabs(fVal)>EPSILON){

					flow[i][j]=fVal;
					fprintf(stdout, "flow[%d,%d]=%.10lf\n",i,j, flow[i][j]);

					//std::cout << "flow["<<i<<","<<j<<"]="<<flow[i][j]<<  "\n";
				} else {
					flow[i][j]=0;
				}
				fscanf(fpOut, "%s", solStatus);

			}

		fclose(fpOut);
		cout << "Finished parsing.\n";

		return 0;
}







