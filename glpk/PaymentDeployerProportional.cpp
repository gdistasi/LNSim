
#include "PaymentDeployerProportional.h"

#include <assert.h>

#include <iostream>

using namespace std;

int  PaymentDeployerProportional::RunSolver(std::vector<std::vector<long>> & flow, long & totalFee) {

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
			fprintf(fpDat, "%lu ", resFunds(i,j));
		}
		fprintf(fpDat, "\n");
	}
	fprintf(fpDat, ";\n\n");

	// base fees
	fprintf(fpDat, "param feerate_perkw:\n");
	for (i = 0; i < nodeNum; i++) {
		fprintf(fpDat, "%d ", i);
	}
	fprintf(fpDat, ":=\n");

			// write down the r matrix
	for (i = 0; i < nodeNum; i++) {
		fprintf(fpDat, "%d ", i);
		for (j = 0; j < nodeNum; j++) {
			fprintf(fpDat, "%lu ", feerate_perkw(i,j));
		}
		fprintf(fpDat, "\n");
	}
	fprintf(fpDat, ";\n\n");

	// base fees
		fprintf(fpDat, "param lowbound:\n");
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
		}
		fprintf(fpDat, ":=\n");

				// write down the r matrix
		for (i = 0; i < nodeNum; i++) {
			fprintf(fpDat, "%d ", i);
			for (j = 0; j < nodeNum; j++) {
				fprintf(fpDat, "%lu ", getLowerBound(i,j));
			}
			fprintf(fpDat, "\n");
		}
		fprintf(fpDat, ";\n\n");




	fprintf(fpDat, "\nparam source := %d;\n\n", source);
	fprintf(fpDat, "param destination := %d;\n\n", destination);
	fprintf(fpDat, "param P := %d;\n\n", payment);

	fprintf(fpDat, "end;\n\n");

	fclose(fpDat);

	sprintf(commandString, "glpsol --model %s --data %s -o %s",
					(modelsDirectory+string("/ModelProportional")).c_str(), dataFile, outputFile);
	cout << commandString << endl;
//	::system(commandString);

//	sprintf(commandString, "glpsol --model %s --data %s -w %s",
	//						"/home/giovanni/windows/workspace/PaymentChannelsSimulator/glpk/Model", dataFile, outputFile);
	//::system(commandString);


	string res=exec(commandString);
	return parseOutputFile(res, outputFile, flow, totalFee);
}


long PaymentDeployerProportional::getLowerBound(int i, int j) {
	if (lowerbound.find(pair<int,int>(i,j))!=lowerbound.end()){
		return lowerbound[pair<int,int>(i,j)];
	} else {
		return 0;
	}
}

long PaymentDeployerProportional::feerate_perkw(int i, int j) {
	if (fees.find(pair<int,int>(i,j))==fees.end())
			return 0;

	return fees[pair<int,int>(i,j)].coefficients[0];
}
