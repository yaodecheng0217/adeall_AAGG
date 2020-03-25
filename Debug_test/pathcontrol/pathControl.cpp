/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 11:09:44
 * @LastEditTime: 2020-03-25 18:26:19
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
	int speed = 50;
	while (1)
	{

		sdata.x = -100;
		sdata.y = 0;
		sdata.yaw = 180 / 57.3;
		while (true)
		{
			time_50ms_Lock.lock();
			if (angle_control_cycle(sdata, 50))
				break;
		}
		get_fork();
		while (1)
		{
			Sleep(1);
		}
		// sdata.x = 500;
		// sdata.y = -1000;
		// sdata.yaw = 270 / 57.3;
		// while (true)
		// {
		// 	time_50ms_Lock.lock();
		// 	if (angle_control_cycle(sdata,-50))
		// 		break;
		// }
		// sdata.x = 0;
		// sdata.y = 0;
		// sdata.yaw = 90 / 57.3;
		// while (true)
		// {
		// 	time_50ms_Lock.lock();
		// 	if (angle_control_cycle(sdata,50))
		// 		break;
		// }
		// sdata.x = 500;
		// sdata.y = -1000;
		// sdata.yaw = 270 / 57.3;
		// while (true)
		// {
		// 	time_50ms_Lock.lock();
		// 	if (angle_control_cycle(sdata,50))
		// 		break;
		// }
	}
}

//路径控制==============算法================================================================
int pathControler::angle_control_cycle(PostionData p, int speed)
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
		double f = CalculationOutputWheelsAngle_F(Position_Error, Angle_Error, speed);
		//printf("---->%f    %f    %f    %f\n", s.yaw * 57.3,Angle_Error*57.3,Position_Error,target_dis);
		//============================================================================
		//==================================输出和目标距离判断============================
		driver->Set_Turn_motor(driver->mode_abs, f, 15);
		if (speed > 0)
		{
			if (target_dis < 230)
			{
				driver->Set_Acc_motor(driver->mode_abs, 0, 15);
				return 1;
			}
			else
			{
				driver->Set_Acc_motor(driver->mode_abs, speed / 30.0, 15);
			}
		}
		else
		{
			if (target_dis < 0)
			{
				driver->Set_Acc_motor(driver->mode_abs, 0, 15);
				return 1;
			}
			else
			{
				driver->Set_Acc_motor(driver->mode_abs, speed / 30.0, 15);
			}
		}
		//============================================================
	}
	else
	{
		printf("code=%s\n", printf_status(code).c_str());
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
			if (angle_control_cycle(sdata, 50))
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
			printf("%f\n", x);
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
			printf("+++++%f\n", x);
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