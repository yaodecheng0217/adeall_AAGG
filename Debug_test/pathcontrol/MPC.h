/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-14 14:56:45
 * @LastEditTime: 2020-03-17 11:36:07
 * @LastEditors: Yaodecheng
 */
#ifndef __MPC_HPP__
#define __MPC_HPP__

struct set_info
{
    double dis = 0;
    double dt;
    double Car_L;
    double set_Q;
    double set_fx;
    double set_fk;
};


struct PostionData
{
    double x=0;
    double y=0;
    double yaw=0;
};
struct ErrorData
{
    double x=0;
    double y=0;
    double yaw=0;
};
//模型预测控制算法
// 输入：角度偏差
//       横向偏差
//       纵向偏差
//       前向距离
// 输出：角度值
double MPCctrloer(ErrorData, set_info,int dis);
double target_determine2(PostionData now, PostionData tar, int Length,int dis);
ErrorData CalculationError(PostionData tar,PostionData now,int dis);
#endif