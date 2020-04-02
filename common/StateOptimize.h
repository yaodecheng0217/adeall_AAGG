#pragma once
#include <Eigen/Dense>
using namespace Eigen;
struct kalInfo
{
	MatrixXf X, P, F, Q, H, R, eye;
};

class StateOptimize
{
public:
	StateOptimize();
	~StateOptimize();
	double raet = 0;
	double Value = 0;
	void run(double x);
	kalInfo info1;
private:
	void fun(kalInfo* info, double input);
	
};

