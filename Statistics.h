#include <iostream>
#include <vector>

class Stats {

public:

	Stats():numPayments(0),successes(0),fails(0),averagePathLength(0),maxFee(0),averageFee(0),feesPaid(0){}

	void print(){
		std::cout << "Payments: " << numPayments << "\n";
		std::cout << "Successes: " << successes << "\n";
		std::cout << "Unsuccesses: " << fails << "\n";
		std::cout << "Fees paid: " << feesPaid << "\n";
		std::cout << "Average fee: " << averageFee << "\n";
		std::cout << "Average path length: " << averagePathLength << "\n";

	}


	int calcPathLength(vector < vector<long> > flows){
		int pl=0;

		for (auto f: flows){
			for (auto p: f){
				if (p!=0) pl++;
			}

		}

		return pl;
	}


	long numPayments;
	double averagePathLength;
	long successes;
	long fails;
	double averageFee;
	long maxFee;
	double feesPaid;

};

