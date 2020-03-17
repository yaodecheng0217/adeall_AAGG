/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 11:09:44
 * @LastEditTime: 2020-03-17 19:06:02
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "pathControler.h"
#include <math.h>

#define pi 3.1415926
void *pathControler::timer_50ms(void *t)
{
    pathControler *p = (pathControler *)t;
    while (true)
    {
        p->time_50ms_Lock.unlock();
        Sleep(50);
    }
}

pathControler::pathControler(APP *dr) : driver(dr)
{
    thread_base timer1(timer_50ms, this);
}

pathControler::~pathControler()
{
}
void pathControler::control_loop()
{
    PostionData sdata;
    sdata.x = 0;
    sdata.y = 0;
    sdata.yaw = 0;
    while (true)
    {
        time_50ms_Lock.lock();
        if (control_cycle(sdata))
        {
            break;
        }
    }
}
int pathControler::control_cycle(PostionData p)
{
    _data::LOCATION_DATA uwb;

    PostionData s;

    set_info info;
    info.Car_L = 300;
    info.dis = 150;
    info.dt = 0.5;
    info.set_fk = 4;
    info.set_fx = 2.45;
    info.set_Q = 0.0001;

    int code = driver->GetData(&uwb, 15);

    if (code == OK)
    {

        s.x = uwb.x;
        s.y = uwb.y;
        s.yaw = uwb.yaw;

       double Angle_Error = CalculationAngleError(p.yaw, s.yaw);
	   double Position_Error = CalculationPositionError(p.x, p.y, p.yaw, s.x, s.y);
	double target_dis = Calculation_target_distance(p.x, p.y, p.yaw, s.x, s.y, 60);
		double f = 0;

		f = CalculationOutputWheelsAngle_F(Position_Error, Angle_Error, 60, target_dis);
        printf("---->%f %f %f %f  %f\n", Position_Error, Position_Error, Angle_Error * 57.3, f * 57.3, target_dis);
        driver->Set_Turn_motor(driver->mode_abs,-f,10);

        if(target_dis<0)
        {
            driver->Set_Acc_motor(driver->mode_abs,0,20);
        }
        else
        {
            driver->Set_Acc_motor(driver->mode_abs,-1,20);
        }
        

    }    
    else
    {
        printf("code=%s\n", printf_status(code).c_str());
    }
    return 0;
}

//@计算纯角度导航输出量
//@输入角度误差
double Calculation_Angle(double Angie_Error, double Position_Error)
{
	
	if (Angie_Error > 180 / Radian_conversion)
		Angie_Error = 360 / Radian_conversion - Angie_Error;
	if (Angie_Error < -180 / Radian_conversion)
		Angie_Error = 360 / Radian_conversion + Angie_Error;

	double Qt = -Angie_Error * 1.5;//-Qfat - Angie_Error;
	if (Qt > pi / 2)
		Qt = pi / 2;
	if (Qt < -pi / 2)
		Qt = -pi / 2;
	return Qt;
}
double pathControler::CalculationOutputWheelsAngle_F(double Position_Error, double Angle_Error, double speed, double L)
{
	double dis = 220;
	double kkk = 80;

	double dis2 = 100;
	double kkk2 = 150;
	double f = 0;
	
	if (speed > 0)
	{
		L = L - 165;
		if (abs(Position_Error) > dis)
		{
			if (Position_Error > 0)
				f = Calculation_Angle(Angle_Error + pi / 2, Position_Error);
			else
				f = Calculation_Angle(Angle_Error - pi / 2, Position_Error);
		}
		else if (abs(Angle_Error) > 30 / Radian_conversion)
			f = Calculation_Angle(Angle_Error, Position_Error);
		else
			f = Calculation_Angle(Angle_Error + atan(Position_Error / (kkk + abs(L) / 2)), Position_Error);
	}
	else
	{
		if (abs(Position_Error) > dis2)
		{
			if (Position_Error > 0)
				f = Calculation_Angle(Angle_Error - pi / 2,0 );
			else
				f = Calculation_Angle(Angle_Error + pi / 2,0);
		}
		else f = Calculation_Angle(Angle_Error - atan(Position_Error / (kkk2 + abs(L)/3)), Position_Error);
		f = -f;
	}
	return f;
}
//计算终点线到当前车直线距离
double pathControler::Calculation_target_distance(double tx, double ty, double tyaw, double sx, double sy, double sv)
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


double pathControler::CalculationAngleError(double target, double starting)
{
	//计算航向偏差
	double Angle_Error = target - starting;
	if (Angle_Error < -pi)
		Angle_Error = Angle_Error + 2 * pi;
	if (Angle_Error > pi)
		Angle_Error = Angle_Error - 2 * pi;
	return Angle_Error;
}
double pathControler::CalculationPositionError(double tx, double ty, double tyaw, double sx, double sy)
{
	//计算夹角
	double theta = tyaw - atan2(ty - sy, tx - sx);
	//计算横向偏差
	double RR = sqrt(((sx - tx) * (sx - tx)) + ((sy - ty) * (sy - ty)));
	double Position_Error = RR * sin(theta);
	return Position_Error;
}