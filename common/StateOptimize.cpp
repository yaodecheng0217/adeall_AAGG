/*
 * @Author: Yaodecheng
 * @Date: 2020-04-01 18:37:04
 * @LastEditTime: 2020-04-08 18:23:27
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "StateOptimize.h"
StateOptimize::StateOptimize()
{
	int Pq = 10;
	int Vq = 100;
	//定义初始状态
	info1.X.resize(2, 1);

	info1.X << 0, 0;
	info1.P.resize(2, 2);
	info1.P << 1, 0, 0, 1;
	info1.F.resize(2, 2);
	info1.F << 1, 0.05, 0, 1;

	info1.Q.resize(2, 2);
	info1.Q << 1.0/Pq, 0, 0, 1.0/Vq;

	info1.H.resize(1, 2);
	info1.H << 1, 0;
	//定义观测噪声协方差
	info1.R.resize(1, 1);
	info1.R << 0.02;
	info1.eye.resize(2, 2);
	info1.eye << 1, 0, 0, 1;
}

StateOptimize::~StateOptimize()
{
	;
}
void StateOptimize::run(double data)
{
	fun(&info1, data);
	Value = info1.X(0, 0);
	raet = info1.X(1, 0);
}
void StateOptimize::fun(kalInfo* info, double input)
{
	MatrixXf x_predict = info->F * info->X;
	MatrixXf p_predict = info->F * info->P * info->F.transpose() + info->Q;
	MatrixXf kalman = p_predict * info->H.transpose() * (info->H * p_predict * info->H.transpose() + info->R).inverse();
	MatrixXf temp(1, 1);
	temp << input;
	info->X = x_predict + kalman * (temp - (info->H * x_predict));
	info->P = (info->eye - kalman * info->H) * p_predict;
}