/*
 * @Author: Yaodecheng
 * @Date: 2020-03-26 10:12:21
 * @LastEditTime: 2020-04-03 16:13:23
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include"ComCTR.h"
#include"mutexlock.hpp"
void ERC212_initoutput();
void sendonedata();
void Set_Lift_motor(double value);
void Set_Acc_motor(double value);
void Set_Turn_motor(double value);
void Set_AUTO(bool value);
double LimitingAmplitude(double in, double max);