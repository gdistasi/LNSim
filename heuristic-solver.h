#include <LightningNetwork.h>

#include <vector>

using namespace std;

class Solver {
	virtual Solver(LightningNetwork * ln) = 0;
	virtual vector<int> solve(unsigned int payment, int source, int destination) = 0;
	virtual vector< vector<int> > solve_mp(unsigned int payment, int source, int destination) = 0;

};



/* Solver in the singlepath case */
class SolverSp {

	virtual Solver(LightningNetwork * _ln):ln(_ln){}

	vector<int> solve(unsigned int payment, int source, int destination);


private:
	LightningNetwork * ln;
};
