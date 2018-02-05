
#include "PaymentDeployerProportional.h"

#include <assert.h>

void PaymentDeployerProportional::AddPaymentChannel(int A, int B, double resFundsA,
		double resFundsB, double sendingFeeA) {

	//channels.insert[pair<int,int>(A,B)]=PaymentChannel(A,B,resFundsA,resFundsB, feeA, feeB);
	//channels.insert(std::map<std::pair<int,int>,PaymentChannel>::value_type(pair<int,int>(A,B),
				    //PaymentChannel(A,B,resFundsA,resFundsB));

	channels[std::pair<int,int>(A,B)]=PaymentChannel(A,B,resFundsA,resFundsB);


	if ( A==source && sendingFeeA != 0 ){
		std::cerr << "Sending fee cannot be different from 0 for the source!\n";
		exit(1);
	}


	propFee[std::pair<int,int>(A,B)]=sendingFeeA;
}


double PaymentDeployerProportional::sendingFee(int x,int y){
		return propFee.find(std::pair<int,int>(x,y))->second;
}

/*
double PaymentDeployerProportional::receivingFee(int x,int y){
	/* we suppose the payment channels are added following the node id order - HACK
	if (x<y){
		return channels.find(pair<int,int>(x,y))->second.receivingFeeB;
	} else {
		return channels.find(pair<int,int>(y,x))->second.receivingFeeA;
	}
}*/


int  PaymentDeployerProportional::RunSolver(std::vector<std::vector<double>> & flow, double & totalFee) {

		int i, j;
		char commandString[1024], solStatus[1024];

		int nodeNum=numNodes;

		/* create temporary files */
		char * dataFile = tempnam(0, "DATA");
		char * outputFile = tempnam(0, "OUT");
		//char * clpModel = tempnam(0, "CLP");

		// open data file
		FILE *fpDat = fopen(dataFile, "w");
		if (fpDat == 0) {
			cout << "failed to open file: " << dataFile << "\n";
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
			/*fprintf(fpDat, "param receivingFee:\n");
			for (i = 0; i < nodeNum; i++) {
				fprintf(fpDat, "%d ", i);
			}
			fprintf(fpDat, ":=\n");

			// write down the r matrix
			/*for (i = 0; i < nodeNum; i++) {
				fprintf(fpDat, "%d ", i);
				for (j = 0; j < nodeNum; j++) {
					fprintf(fpDat, "%.8lf ", receivingFee(i,j));
				}
				fprintf(fpDat, "\n");
			}
			fprintf(fpDat, ";\n\n");*/



		fprintf(fpDat, "param source := %d;\n\n", source);
		fprintf(fpDat, "param destination := %d;\n\n", destination);
		fprintf(fpDat, "param P := %.8lf;\n\n", payment);

		fprintf(fpDat, "end;\n\n");

		fclose(fpDat);


		sprintf(commandString, "glpsol --model %s --data %s -w %s",
						(modelsDirectory+string("/Model")).c_str(), dataFile, outputFile);
		std::cout << commandString << std::endl;
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


