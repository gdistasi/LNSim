/*
 * utils.cpp
 *
 *  Created on: 19/lug/2016
 *      Author: giovanni
 */

#include "defs.h"
#include <iostream>

double mytrunc(double a){
	long p = (a / MINIMUM_UNIT);
	//std::cout << p << "p\n";

	return double(p)*MINIMUM_UNIT;
}
