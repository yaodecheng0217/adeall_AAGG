/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 10:55:58
 * @LastEditTime: 2020-03-17 18:34:40
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence
 */


#include"../APP/app.h"
#include "MPC.h"
#ifndef _PATH_CONTROL_H_
#define _PATH_CONTROL_H_

#define Radian_conversion 57.3
struct state
{
	double x;
	double y;
	double yaw;
	double v;
};
class pathControler
{
private:
   APP *driver;
public:
    pathControler( APP *dr);
    ~pathControler();
void control_loop();
private:
    MutexLock time_50ms_Lock;
    static void *timer_50ms(void *);
    
    int control_cycle(PostionData sdata);



	double CalculationOutputWheelsAngle_F(double Position_Error, double Angle_Error,double speed,double L);
	double CalculationPositionError(double tx, double ty, double tyaw, double sx, double sy);
	double CalculationAngleError(double target, double starting);
	
	//计算终点线到当前车直线距离
	double Calculation_target_distance(double tx, double ty, double tyaw, double sx, double sy,double sv);

};


#endif


