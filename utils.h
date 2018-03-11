#include<string>
#include <fstream>

#include <regex>
#include <vector>
#include <iostream>

using namespace std;

string token(string str, int index, char sep=' ');
vector<string> tokenize(string str);
long convertTo(string str);
double convertToDouble(string str);

void printSolution(ostream & of, string title, vector< vector<long>> flows, long fee );
void printSolution(ostream & of, string title, vector<Tpath> paths, long fee);
