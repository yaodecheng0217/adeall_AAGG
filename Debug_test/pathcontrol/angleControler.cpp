/*
 * @Author: Yaodecheng
 * @Date: 2020-03-18 11:01:49
 * @LastEditTime: 2020-03-19 18:50:13
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "angleControler.h"

//@计算纯角度导航输出量
//@输入角度误差
double Calculation_Angle(double Angie_Error, double Position_Error)
{

    if (Angie_Error > 180 / Radian_conversion)
        Angie_Error = 360 / Radian_conversion - Angie_Error;
    if (Angie_Error < -180 / Radian_conversion)
        Angie_Error = 360 / Radian_conversion + Angie_Error;

    double Qt = -Angie_Error * 1.5; //-Qfat - Angie_Error;
    if (Qt > pi / 2)
        Qt = pi / 2;
    if (Qt < -pi / 2)
        Qt = -pi / 2;
    return Qt;
}
double CalculationOutputWheelsAngle_F(double Position_Error, double Angle_Error, double speed, double L)
{
    double dis = 160;
    double kkk = 150;

    double dis2 = 100;
    double kkk2 = 150;
    double f = 0;

    if (speed > 0)
    {
        if (abs(L) > dis)
        {
            if (Position_Error > 0)
                f = Calculation_Angle(Angle_Error + pi / 2, Position_Error);
            else
                f = Calculation_Angle(Angle_Error - pi / 2, Position_Error);
        }
        else if (abs(Angle_Error) > 20 / Radian_conversion)
            f = Calculation_Angle(Angle_Error, Position_Error);
        else
            //f = Calculation_Angle(Angle_Error, Position_Error);
            f = Calculation_Angle(Angle_Error + atan(Position_Error/100) , Position_Error);
    }
    else
    {
        if (abs(Position_Error) > dis2)
        {
            if (Position_Error > 0)
                f = Calculation_Angle(Angle_Error - pi / 2, 0);
            else
                f = Calculation_Angle(Angle_Error + pi / 2, 0);
        }
        else
            f = Calculation_Angle(Angle_Error - atan(Position_Error / (kkk2 + abs(L) / 3)), Position_Error);
        f = -f;
    }
    return f;
}
//计算终点线到当前车直线距离
double Calculation_target_distance(double tx, double ty, double tyaw, double sx, double sy, double sv)
{
    int RR = sqrt(((sx - tx) * (sx - tx)) + ((sy - ty) * (sy - ty)));
    //向量法求
    //先求目标角度
    double yawc = tyaw + pi / 2;
    //计算夹角
    double theta = yawc - atan2(ty - sy, tx - sx);
    //计算投影
    double length = RR * sin(theta);
    if (sv > 0)
    {
        length = -length;
    }
    return length;
}

double CalculationAngleError(double target, double starting)
{
    //计算航向偏差
    double Angle_Error = target - starting;
    if (Angle_Error < -pi)
        Angle_Error = Angle_Error + 2 * pi;
    if (Angle_Error > pi)
        Angle_Error = Angle_Error - 2 * pi;
    return Angle_Error;
}
double CalculationPositionError(double tx, double ty, double tyaw, double sx, double sy)
{
    //计算夹角
    double theta = tyaw - atan2(ty - sy, tx - sx);
    //计算横向偏差
    double RR = sqrt(((sx - tx) * (sx - tx)) + ((sy - ty) * (sy - ty)));
    double Position_Error = RR * sin(theta);
    return Position_Error;
}