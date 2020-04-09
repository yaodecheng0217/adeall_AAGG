/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 10:55:58
 * @LastEditTime: 2020-04-09 16:52:21
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence
 */

#include "../APP/app.h"
#include "MPC.h"
#include "angleControler.h"
#ifndef _PATH_CONTROL_H_
#define _PATH_CONTROL_H_

class pathControler
{
private:
public:
    APP *driver;
    pathControler(APP *dr);
    ~pathControler();
    void control_loop();
    void control_loop2();
    void control_loop3();
    void control_loop4();

    void Tracking_arc(double R, double x, double y);

    int point_2_point(double x, double y, double yaw, double speed, bool &shoutdown, bool isend);
    void Catstop();
    void getup();
    void putdown();

private:
    MutexLock time_50ms_Lock;
    static void *timer_50ms(void *);
    static void *timer_1000ms(void *);
    int angle_control_cycle(PostionData sdata, double speed, bool is_end);
    int outputcontrol(double target_dis, double turnAngle, double speed);
    double limit(double in, double max);
    int get_fork();
};

#endif
