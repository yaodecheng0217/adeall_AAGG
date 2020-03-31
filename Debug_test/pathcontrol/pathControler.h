/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 10:55:58
 * @LastEditTime: 2020-03-25 15:07:14
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence
 */


#include"../APP/app.h"
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
void getup();
void putdown();
private:
    MutexLock time_50ms_Lock;
    static void *timer_50ms(void *);
    static void *timer_1000ms(void *);
    int angle_control_cycle(PostionData sdata,int speed);
    int get_fork();
};


#endif


