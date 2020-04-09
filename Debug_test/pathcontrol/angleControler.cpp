/*
 * @Author: Yaodecheng
 * @Date: 2020-03-18 11:01:49
 * @LastEditTime: 2020-04-09 18:47:46
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "angleControler.h"
#include "StateOptimize.h"
#include "stdio.h"
#include "PID.h"
#include "readconfig/readjson.h"
#define pi 3.1415926
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

    double Qt = -Angie_Error * 1; //-Qfat - Angie_Error;

    if (Qt > pi / 2)
        Qt = pi / 2;
    if (Qt < -pi / 2)
        Qt = -pi / 2;
    return Qt;
}
double limit(double in, double max)
{
    if (in > max)
        return max;
    if (in < -max)
        return -max;
    return in;
}
PID_IncTypeDef incpidinfo;
PID_IncTypeDef tfinfo;
PID_IncTypeDef yawinfo;
double buchang = 0;
double fout = 0;
double yawErr = 0;
StateOptimize run;
ReadSeting set("contrlseting.json");
double maxspeed=100;
void CalculationOutputWheelsAngle_F(double Position_Error, double Angle_Error, double &speed, double &turnangle)
{
    system("cls");
    printf("%f %f ", Position_Error, Angle_Error);
    run.run(Position_Error);
    Position_Error = run.Value;
    //避免过大的冲击
    incpidinfo.Kp = 0.08;
    incpidinfo.Kd = 0.1;
    //轮子控制死区，静态误差控制
    tfinfo.Kp = 0.00004;
    tfinfo.Ki = 0.00002;

    yawinfo.Kp = 0.0000; //0.0002

    //接近轨道阈值，超过此阈值将直接将车垂直九十度先接近轨道
    double front_FL = 138; // double front_FL = 138;
    double rear_FL = 70;   //70
    //前视距离
    double front_CL = 190; //190
    double rear_CL = 0;    //140
    double Xf = 30;
    double Xb = 30;

    double rangek = 100;

    set.reload();
    bool print = false;
    set.GetValue("print", print);
    set.setprint(print);
    set.GetValue("turn_kp", incpidinfo.Kp);
    set.GetValue("turn_ki", incpidinfo.Ki);
    set.GetValue("turn_kd", incpidinfo.Kd);
    set.GetValue("turnerror_kp", incpidinfo.Kp);
    set.GetValue("turnerror_ki", incpidinfo.Ki);
    set.GetValue("turnerror_kd", incpidinfo.Kd);
    set.GetValue("front_FL", front_FL);
    set.GetValue("rear_FL", rear_FL);
    set.GetValue("front_CL", front_CL);
    set.GetValue("rear_CL", rear_CL);
    set.GetValue("xf", Xf);
    set.GetValue("xb", Xb);
    set.GetValue("rangeK", rangek);
    //
    double f = 0;
    //Angle_Error = Angle_Error + yawErr;
    if (speed > 0)
    {
        //换算前叉误差
        double Ferr = (Position_Error / sin(Angle_Error) - front_CL) * sin(Angle_Error);

        printf(" tarA=%f   tarP=%f ", Angle_Error * 57.3, Ferr);
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
            f = Calculation_Angle(Angle_Error - atan(Ferr / Xf));
            if (abs(Angle_Error) < 10 / 57.3 && abs(Ferr) < 10)
            {
                buchang = buchang + PID_Inc(0, -Ferr, &tfinfo);
                if (abs(Ferr) < 5)
                    yawErr = yawErr + PID_Inc(0, Angle_Error, &yawinfo);
            }
        }
    }
    else
    {
        double Berr = (Position_Error / sin(Angle_Error) - rear_CL) * sin(Angle_Error);
        double Berr2 = (Position_Error / sin(Angle_Error) - 140) * sin(Angle_Error);
        printf(" tarA=%f   tarP=%f ", Angle_Error * 57.3, Position_Error);
        if (abs(Berr2) > rear_FL)
        {
            if (Berr2 < 0)
                f = Calculation_Angle(Angle_Error - pi / 2);
            else
                f = Calculation_Angle(Angle_Error + pi / 2);
        }
        else
        {
            f = Calculation_Angle(Angle_Error + atan(Berr / Xb));
        }

        if (abs(Angle_Error) < 5 / 57.3 && abs(Berr) < 20)
        {
            buchang = buchang + PID_Inc(0, -Berr, &tfinfo);
            if (abs(Berr) < 10)
                yawErr = yawErr + PID_Inc(0, Angle_Error, &yawinfo);
        }

        f = -f;
    }
    if (buchang > 10 / 57.3)
        buchang = 10 / 57.3;
    if (buchang < -10 / 57.3)
        buchang = -10 / 57.3;
    //printf("## out=%f  tarf=%f  yawerr=%f  ", lastf * 57.3, f * 57.3, yawErr * 57.3);

    double out = buchang + turnangle + PID_Inc(f, turnangle, &incpidinfo);
    double turnrange = abs((out - turnangle) * rangek);

    double maxerr=abs(limit(Position_Error, 90));
    f=limit(f,1);
    double max = 90-abs(50*asin(f))+ 10;
    if(maxspeed<max)
    {
        maxspeed=maxspeed+0.3;
    }
    else
    {
        maxspeed=max;
    }
    
    printf("turnrange= %f##", maxspeed);
    speed = limit(speed, maxspeed);
    turnangle = out;
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