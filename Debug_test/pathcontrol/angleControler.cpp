/*
 * @Author: Yaodecheng
 * @Date: 2020-03-18 11:01:49
 * @LastEditTime: 2020-03-24 14:52:12
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "angleControler.h"
#include"stdio.h"
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
    //接近轨道阈值，超过此阈值将直接将车垂直九十度先接近轨道
    double dis = 160;
    double dis2 = 100;
    //
    double f = 0;

    if (speed > 0)
    {

        //换算前叉误差
        double Ferr= (Position_Error/sin(Angle_Error)+200)*sin(Angle_Error);
        printf("%f   %f===\n",Position_Error,Ferr);
        if (abs(Position_Error) > dis)
        {
            if (Ferr > 0)
                f = Calculation_Angle(Angle_Error + pi / 2, Ferr);
            else
                f = Calculation_Angle(Angle_Error - pi / 2, Ferr);
        }
        else if (abs(Angle_Error) > 60 / Radian_conversion)
            f = Calculation_Angle(Angle_Error, Ferr);
        else
            //f = Calculation_Angle(Angle_Error, Position_Error);
            f = Calculation_Angle(Angle_Error + atan(Ferr/(20+abs(speed/2))) , Ferr);
    }
    else
    {
        //换算前叉误差
        double Berr= (Position_Error/sin(Angle_Error)+90)*sin(Angle_Error);
        if (abs(Position_Error) > dis2)
        {
            if (Berr > 0)
                f = Calculation_Angle(Angle_Error - pi / 2, 0);
            else
                f = Calculation_Angle(Angle_Error + pi / 2, 0);
        }
        else
            f = Calculation_Angle(Angle_Error - atan(Berr / (30 + abs(speed/2))), Berr);
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