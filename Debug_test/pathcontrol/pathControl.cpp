/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 11:09:44
 * @LastEditTime: 2020-03-22 18:12:54
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "pathControler.h"



void *pathControler::timer_50ms(void *t)
{
	pathControler *p = (pathControler *)t;
	while (true)
	{
		p->time_50ms_Lock.unlock();
		Sleep(50);
	}
}
void *pathControler::timer_1000ms(void *t)
{
	pathControler *p = (pathControler *)t;
	while (true)
	{   //p->driver->printf__RecallList();
		Sleep(1000);
	}
}

pathControler::pathControler(APP *dr) : driver(dr)
{
	thread_base timer1(timer_50ms, this);
	thread_base timer2(timer_1000ms, this);
}

pathControler::~pathControler()
{
}
void pathControler::control_loop()
{
	PostionData sdata;
	while (1)
	{

		sdata.x = 0;
		sdata.y = -1000;
		sdata.yaw = 90 / 57.3;
		while (true)
		{
			time_50ms_Lock.lock();
			if (angle_control_cycle(sdata))
				break;
		}
		sdata.x = 500;
		sdata.y = -1000;
		sdata.yaw = 180 / 57.3;
		while (true)
		{
			time_50ms_Lock.lock();
			if (angle_control_cycle(sdata))
				break;
		}
		sdata.x = 500;
		sdata.y = 1500;
		sdata.yaw = 270 / 57.3;
		while (true)
		{
			time_50ms_Lock.lock();
			if (angle_control_cycle(sdata))
				break;
		}
	}
}

//路径控制==============算法================================================================
int pathControler::angle_control_cycle(PostionData p)
{
	static int cnt=0;
	LOCATION_DATA uwb;

	PostionData s;

	set_info info;
	info.Car_L = 200;
	info.dis = 150;
	info.dt = 0.5;
	info.set_fk = 4;
	info.set_fx = 2.45;
	info.set_Q = 0.0001;

	int code = driver->GetData(&uwb, 105);
	if (code == OK)
	{

		s.x = uwb.x;
		s.y = uwb.y;
		s.yaw = uwb.yaw;

		double Front_x = uwb.x - 140 * cos(uwb.yaw);
		double Front_y = uwb.y - 140 * sin(uwb.yaw);

		s.x = Front_x;
		s.y = Front_y;

		double Angle_Error = CalculationAngleError(p.yaw, s.yaw);
		double Position_Error = CalculationPositionError(p.x, p.y, p.yaw, s.x, s.y);
		double Position_Error_B = CalculationPositionError(p.x, p.y, p.yaw, uwb.x, uwb.y);
		double target_dis = Calculation_target_distance(p.x, p.y, p.yaw, s.x, s.y, 60);
		double f = 0;
     
		f = CalculationOutputWheelsAngle_F(Position_Error, Angle_Error, 60, Position_Error_B);
		printf("---->%f %f %f %f  %f\n", Position_Error_B, Position_Error, Angle_Error * 57.3, f * 57.3, target_dis);
		driver->Set_Turn_motor(driver->mode_abs, f, 15);
		if (target_dis < 0)
		{
			driver->Set_Acc_motor(driver->mode_abs, 0, 15);
			return 1;
		}
		else
		{
			driver->Set_Acc_motor(driver->mode_abs, 1.5, 15);
		}
	}
	else
	{
		printf("code=%s\n", printf_status(code).c_str());
	}
	return 0;
}
