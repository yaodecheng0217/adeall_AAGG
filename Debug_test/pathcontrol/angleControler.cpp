/*
 * @Author: Yaodecheng
 * @Date: 2020-03-18 11:01:49
 * @LastEditTime: 2020-03-31 17:44:17
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "angleControler.h"
#include "stdio.h"
#include "PID.h"
//@计算纯角度导航输出量
//@输入角度误差
double Calculation_Angle(double Angie_Error)
{
    static PID_IncTypeDef ang;
    static double bu = 0;
    ang.Kp = 0.01;
    ang.Ki = 0;

    if (Angie_Error > 180 / Radian_conversion)
        Angie_Error = 360 / Radian_conversion - Angie_Error;
    if (Angie_Error < -180 / Radian_conversion)
        Angie_Error = 360 / Radian_conversion + Angie_Error;
   
    
    //printf("<<pid err= %f %f>>", Angie_Error * 57.3, bu * 57.3);
    double Qt = -Angie_Error * 1.5; //-Qfat - Angie_Error;
   
    if (Qt > pi / 2)
        Qt = pi / 2;
    if (Qt < -pi / 2)
        Qt = -pi / 2;
    return Qt;
}
PID_IncTypeDef incpidinfo;
PID_IncTypeDef tfinfo;
double buchang = 0;
double fout = 0;
double CalculationOutputWheelsAngle_F(double Position_Error, double Angle_Error, double speed, double lastf)
{

    incpidinfo.Kp = 0.08;
    tfinfo.Kp = 0.00001;

    // 接近轨道阈值，超过此阈值将直接将车垂直九十度先接近轨道
    // double front_FL = 200;
    // double rear_FL = 20;
    // 前视距离
    // double front_CL = 180;
    // double rear_CL = 120;

    //接近轨道阈值，超过此阈值将直接将车垂直九十度先接近轨道
    double front_FL = 138;
    double rear_FL = 70;
    //前视距离
    double front_CL = 190;
    double rear_CL = 140;

    //
    double f = 0;

    if (speed > 0)
    {

        //换算前叉误差
        double Ferr = (Position_Error / sin(Angle_Error) - front_CL) * sin(Angle_Error);

        printf("%f  %f   %f===>\n", Angle_Error * 57.3, Position_Error, Ferr);
        if (abs(Position_Error) > front_FL)
        {
            if (Position_Error > 0)
                f = Calculation_Angle(Angle_Error - pi / 2);
            else
                f = Calculation_Angle(Angle_Error + pi / 2);
        }
        else
        {
            //printf("???");
            f = Calculation_Angle(Angle_Error - atan(Ferr / (30)));
            if (abs(Angle_Error) < 10 / 57.3 && abs(Ferr) < 10)
                buchang = buchang + PID_Inc(0, -Ferr, &tfinfo);
        }
    }
    else
    {
        //换算前叉误差
        double Berr = (Position_Error / sin(Angle_Error) - rear_CL) * sin(Angle_Error);
        printf("%f   %f   %f===\n", Angle_Error * 57.3, Position_Error, Berr);
        if (abs(Berr) > rear_FL)
        {
            if (Berr < 0)
                f = Calculation_Angle(Angle_Error - pi / 2);
            else
                f = Calculation_Angle(Angle_Error + pi / 2);
        }
        else if (abs(Angle_Error) > 60 / Radian_conversion)
            f = Calculation_Angle(Angle_Error);
        else
            f = Calculation_Angle(Angle_Error + atan(Position_Error / (60)));
        //if(abs(Position_Error)<15&&abs(Angle_Error)<10/57.3)
        //buchang=buchang+PID_Inc(0,-Position_Error,&tfinfo);
        //if((Angle_Error)<10/57.3&& abs(Position_Error)<20)
        //buchang=buchang+PID_Inc(0,-Position_Error,&tfinfo);

        f = -f;
    }
    printf(" %f ==   %f ", lastf * 57.3, buchang * 57.3);
    fout = lastf + PID_Inc(f, lastf, &incpidinfo);
    return fout + buchang;
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
    if (sv < 0)
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