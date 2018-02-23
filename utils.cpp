/*
 * utils.cpp
 *
 *  Created on: 19/lug/2016
 *      Author: giovanni
 */

#include "defs.h"
#include "utils.h"
#include <iostream>

double mytrunc(double a){
	long p = (a / MINIMUM_UNIT);
	//std::cout << p << "p\n";

	return double(p)*MINIMUM_UNIT;
}

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
