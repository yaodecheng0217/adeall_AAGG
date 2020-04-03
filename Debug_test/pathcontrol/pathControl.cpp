/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 11:09:44
 * @LastEditTime: 2020-04-03 12:04:24
 * @LastEditors: Yaodecheng
 * @Description: 路径控制层
 *   ^
 *   |y  90°
 *   |
 *   |   <=
 * --|---------->x  0°
 * 
 * 坐标定义：以x轴为零度方向，逆时针转90度为y轴正方向
 * 车身定义：货叉方向为驱动正方向,也是车身航向参考正方向
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
	{ //p->driver->printf__RecallList();
		Sleep(10);
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
	int speed = 60;
	bool contrl = 1;
	while (1)
	{
		point_2_point(-460, 50, 180 / 57.3, speed, contrl, true);
		Catstop();
		printf("path is ok!!\n");
		putdown();
		point_2_point(300, 50, 0 / 57.3, -speed, contrl, true);
		Catstop();
		printf("path is ok22!!\n");
		Sleep(5000);
	}
}
void pathControler::control_loop2()
{
	PostionData sdata;
	int speed = 78;
	bool contrl = true;
	while (1)
	{
		point_2_point(-460, 50, 180 / 57.3, speed, contrl, 1);
		Catstop();
		getup();
		point_2_point(300, 50, 0 / 57.3, -speed, contrl, false);
		//===================>
		point_2_point(0, -250, 270 / 57.3, speed, contrl, true);
		Catstop();
		putdown();
		//============
		//point_2_point(0, 200, 90 / 57.3, -50, contrl, false);
		point_2_point(300, 50, 0 / 57.3, -50, contrl, false);
		Catstop();
		Sleep(5000);
		point_2_point(0, -250, 270 / 57.3, speed, contrl, true);
		Catstop();
		getup();
		point_2_point(0, 225, 90 / 57.3, -60, contrl, false);
		//point_2_point(300, 50, 0 / 57.3, -speed, contrl, false);
		point_2_point(-460, 50, 180 / 57.3, speed, contrl, true);
		Catstop();
		putdown();
		point_2_point(300, 50, 0 / 57.3, -speed, contrl, false);
		Catstop();
		Sleep(10000);
	}
}
//路径控制==============算法================================================================
int pathControler::angle_control_cycle(PostionData p, int speed, bool is_end)
{
	static int cnt = 0;
	LOCATION_DATA uwb;

	PostionData s;

	int code = driver->GetData(&uwb, 20);
	if (code == OK)
	{
		//======================获取数据=======
		int noise = 0;
		if (noise == 0)
		{
			s.x = uwb.x;
			s.y = uwb.y;
		}
		else
		{
			s.x = uwb.x + rand() % abs(noise * 2) - noise;
			s.y = uwb.y + rand() % abs(noise * 2) - noise;
		}
		s.yaw = uwb.yaw;
		//============================================================================
		//============================算法计算========================================
		//double speed = 50;
		if (speed < 0)
		{
			p.yaw = p.yaw + 180 / 57.3;
			if (p.yaw > 360 / 57.3)
			{
				p.yaw = p.yaw - 360 / 57.3;
			}
		}

		double Angle_Error = CalculationAngleError(p.yaw, s.yaw);
		double Position_Error = CalculationPositionError(p.x, p.y, p.yaw, s.x, s.y);
		double target_dis = Calculation_target_distance(p.x, p.y, p.yaw, s.x, s.y, speed);

		double getF;
		driver->GetData(Type_TurnAngleValue, &getF);
		//printf("get wheel %f\n",getF*57.3);
		double f = CalculationOutputWheelsAngle_F(Position_Error, Angle_Error, speed, getF);
		//printf("---->%f    %f    %f    %f\n", f * 57.3, Angle_Error * 57.3, Position_Error, target_dis);
		//============================================================================
		//==================================输出和目标距离判断============================
		if (is_end)
		{
			double max = 100;
			if (speed > 0 && target_dis - 170 < 50)
				max = (target_dis - 170) / 8 + 35;
			if (speed < 0 && target_dis - 80 < 50)
				max = (target_dis - 80) / 8 + 40;

			speed = limit(speed, max);
		}
		
		return outputcontrol(target_dis, f, speed);
		//============================================================
	}
	else
	{
		printf("code=%s\n", printf_status(code).c_str());
	}
	printf("\n");
	return 0;
}
double pathControler::limit(double in, double max)
{
	if (in > max)
		return max;
	if (in < -max)
		return -max;
	return in;
}
int pathControler::outputcontrol(double target_dis, double turnAngle, double speed)
{
	driver->Set_Turn_motor(driver->mode_abs, turnAngle, 15);
	double maxspeed = 50 + 50 * cos(abs(turnAngle));
	speed = limit(speed, maxspeed);
	printf("speed= %f  turn= %f\n", speed,turnAngle*57.3);
	if (speed > 0)
	{
		if (target_dis < 170)
		{
			return 1;
		}
		else
		{
			driver->Set_Acc_motor(driver->mode_abs, speed / 100, 15);
		}
	}
	else
	{
		if (target_dis < 80)
		{
			return 1;
		}
		else
		{
			driver->Set_Acc_motor(driver->mode_abs, speed / 100, 15);
		}
	}
	return 0;
}
int pathControler::point_2_point(double x, double y, double yaw, double speed, bool &shoutdown, bool isend)
{
	PostionData sdata;
	sdata.x = x;
	sdata.y = y;
	sdata.yaw = yaw;
	while (shoutdown)
	{
		time_50ms_Lock.lock();
		if (angle_control_cycle(sdata, speed, isend))
			return 1;
	}
	return 0;
}
int pathControler::get_fork()
{
	LOCATION_DATA uwb;

	PostionData sdata;

	int code = driver->GetData(&uwb, 20);
	if (code == OK)
	{
		sdata.x = uwb.x + 3000 * cos(180 / 57.3);
		sdata.y = uwb.y + 3000 * sin(180 / 57.3);
		sdata.yaw = 180 / 57.3;
		while (true)
		{
			time_50ms_Lock.lock();
			if (angle_control_cycle(sdata, 50, true))
				break;
			double x;
			code = driver->GetData(DATA_SET_GET_TYPE_LIST::Type_TrayL_lasser, &x, 100);
			if (code == OK)
			{
				printf("%f\n", x);
				if (x < 0.9)
					break;
			}
		}
	}
	driver->Set_Turn_motor(driver->mode_abs, 0, 15);
	driver->Set_Acc_motor(driver->mode_abs, 0, 15);
	while (true)
	{
		time_50ms_Lock.lock();
		driver->Set_Forward_motor(driver->mode_abs, 0.1, 15);
		double x;
		code = driver->GetData(DATA_SET_GET_TYPE_LIST::Type_TrayL_lasser, &x, 100);
		if (code == OK)
		{
			if (x < 0.1)
				break;
		}
	}
	driver->Set_Forward_motor(driver->mode_abs, 0, 15);

	while (true)
	{
		time_50ms_Lock.lock();
		driver->Set_Lift_motor(driver->mode_abs, 0.1, 15);
		double x;
		code = driver->GetData(DATA_SET_GET_TYPE_LIST::Type_high_lasser, &x, 100);
		if (code == OK)
		{
			if (x > 1)
				break;
		}
	}
	driver->Set_Lift_motor(driver->mode_abs, 0, 15);
	while (true)
	{
		time_50ms_Lock.lock();
		driver->Set_Forward_motor(driver->mode_abs, -0.1, 15);
		double x;
		code = driver->GetData(DATA_SET_GET_TYPE_LIST::Type_forward_lasser, &x, 100);
		if (code == OK)
		{
			printf("%f\n", x);
			if (x < 0.03)
				break;
		}
	}
	driver->Set_Forward_motor(driver->mode_abs, 0, 15);
	exit(0);
	return 0;
}

void pathControler::getup()
{
	for (size_t i = 0; i < 70; i++)
	{
		driver->Set_Lift_motor(driver->mode_abs, 0.65);
		Sleep(50);
	}
	driver->Set_Lift_motor(driver->mode_abs, 0);
}

void pathControler::putdown()
{
	for (size_t i = 0; i < 200; i++)
	{
		driver->Set_Lift_motor(driver->mode_abs, -0.4);
		Sleep(50);
	}
	driver->Set_Lift_motor(driver->mode_abs, 0);
}

void pathControler::Tracking_arc(double R, double x, double y)
{
	int speed = 50;
	bool contrl = 1;
	while (1)
	{
		for (size_t i = 0; i < 360; i++)
		{
			double yaw = (i + 90);
			if (yaw < 0)
				yaw = yaw + 360;
			if (yaw > 360)
				yaw = yaw - 360;
			point_2_point(x + R * cos(i / 57.3), y + R * sin(i / 57.3), yaw / 57.3, -speed, contrl, false);
			//Sleep(5000);
			printf("i=%d\n", i);
		}
		Sleep(5000);
		printf("ok!\n");
	}
}
void pathControler::Catstop()
{
	while (driver->Set_Acc_motor(driver->mode_abs,0)!=OK)
	{
		Sleep(1);
	}
}