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

using namespace std;


double PaymentDeployer::resFunds(int x,int y){
	//std::cout << "FINDING " << channels.find( pair<int,int>(x,y))->second.resFundsA  ;
	return channels.find( pair<int,int>(x,y) )->second.resFundsA;
}


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






