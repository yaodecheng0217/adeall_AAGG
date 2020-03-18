/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 10:55:58
 * @LastEditTime: 2020-03-18 11:14:17
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
   APP *driver;
public:
    pathControler( APP *dr);
    ~pathControler();
void control_loop();
private:
    MutexLock time_50ms_Lock;
    static void *timer_50ms(void *);
    
    int angle_control_cycle(PostionData sdata);
};


#endif


