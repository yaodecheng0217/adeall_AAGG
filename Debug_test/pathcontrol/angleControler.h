/*
 * @Author: Yaodecheng
 * @Date: 2020-03-18 11:00:29
 * @LastEditTime: 2020-03-18 16:49:36
 * @LastEditors: Yaodecheng
 * @Description: 角度补偿控制算法，稳定，但接近慢
 * @Adeall licence@2020
 */
#ifndef _ANGLE_CONTROLER_H_
#define _ANGLE_CONTROLER_H_
#include <math.h>


#define pi 3.1415926
#define Radian_conversion 57.3
struct state
{
    double x;
    double y;
    double yaw;
    double v;
};
double CalculationOutputWheelsAngle_F(double Position_Error, double Angle_Error, double speed, double L);
double CalculationPositionError(double tx, double ty, double tyaw, double sx, double sy);
double CalculationAngleError(double target, double starting);
//计算终点线到当前车直线距离
double Calculation_target_distance(double tx, double ty, double tyaw, double sx, double sy, double sv);
#endif