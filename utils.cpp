/*
 * utils.cpp
 *
 *  Created on: 19/lug/2016
 *      Author: giovanni
 */


#include "utils.h"
#include <iostream>
#include <sstream>
#include <vector>


using namespace std;


long convertTo(string str){
	istringstream buffer(str);
	long value;
	buffer >> value;
	return value;
}

double convertToDouble(string str){
	istringstream buffer(str);
	double value;
	buffer >> value;
	return value;
}

string token(string str, int index, char sep){
	int i=0;
	int k=0;
	std::stringstream ss;

	// skip initial seps
	while (i < str.size() && str[i]==sep)
		i++;

	while (i < str.size() && k < index){
		if (str[i]==' '){

			k++;

			while (i < str.size() && str[i]==sep)
				i++;

			if (k==index) break;
		}

		i++;
	}

	while (i < str.size() && str[i]!=sep){
		ss << str[i];
		i++;
	}


	return ss.str();
}

//#ifndef TESTUNIT0

vector<string> tokenize(string str)
{

	vector<string> result;

    regex reg("\\s+");

    sregex_token_iterator iter(str.begin(), str.end(), reg, -1);
    sregex_token_iterator end;

    vector<string> vec(iter, end);

    //std::cout << "\nString: " << str << "\n";
    //std::cout << "Tokens: ";
    int i=0;

    for (auto a : vec)
    {
    		//std::cout << "--" << i << ":" << a << "--";
	        result.push_back(a);
	        i++;
    }

    //std::cout << "\n";



    return result;
}

void printSolution(ostream & of, string title, vector<vector <long>> flows, long fee){
	of << title << "\n";

	of << "Total fees=" << fee << "\n";
	for (int i=0; i<flows.size(); i++){
		for (int j=0; j<flows.size(); j++){
			if (flows[i][j]!=0)
				of << "flows[" << i << "," << j << "]=" << flows[i][j] << ";\n";
		}
	}

}

void printSolution(ostream & of, string title, vector<Tpath> paths, long fee){
	of << title << "\n";

	of << "Total fees=" << fee << "\n";
	for (auto pv: paths){
		of << "Path: \n";
		for (auto p: pv){
			of << "flow[" << p.first.first << "," << p.first.second << "]=" << p.second << ";\n";
		}
	}

}

double mytrunc(double a){
	long p = (a / MINIMUM_UNIT);
	//std::cout << p << "p\n";

	return double(p)*MINIMUM_UNIT;
}


//#endif


#ifdef TESTUNIT0
int main(){

	string a = "j 33457 362736664.891348";

	std::cout << convertToDouble(token(a,0)) << endl;
	std::cout << convertToDouble(token(a,1))<< endl;
	std::cout << convertToDouble(token(a,2))<< endl;
}
#endif

