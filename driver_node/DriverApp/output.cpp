/*
 * @Author: Yaodecheng
 * @Date: 2020-03-26 10:12:45
 * @LastEditTime: 2020-04-07 16:15:07
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "output.h"
#include "thread_base.h"
ComCTR x;
struct DC
{
    double acc = 0;
    double turn = 0;
    double litf = 0;
    bool ao = 0;
} dc;
double LimitingAmplitude(double in, double max)
{
    if (in > max)
    {
        return max;
    }
    if (in < -max)
    {
        return -max;
    }
    return in;
}
void outcallback(int L, string str, void *)
{
    
    //printf("recv=%s\n", str.c_str());
}
void *contrlloop(void *)
{
    while (1)
    {
        Sleep(100);
        sendonedata();
    }
}
void ERC212_initoutput()
{
    x.setCallBack(outcallback, NULL);
    x.Open("\\\\.\\COM12", 115200);
    thread_base T(contrlloop,NULL);
}
void sendonedata()
{
    std::string famt;
    if (dc.ao == true)
    {
        famt = "@A=" + to_string(dc.ao) + ",E=1" + ",W=" + to_string(dc.acc) + ",R=" + to_string(dc.litf) + ",T=" + to_string(dc.turn) + ",#";
    }
    else
    {
        famt = "@A=" + to_string(dc.ao) + ",E=0" + ",W=" + to_string(dc.acc) + ",R=" + to_string(dc.litf) + ",T=" + to_string(dc.turn) + ",#";
    }
    //printf("===>%s\n",famt.c_str());
    if (x.comstate())
    {
        x.WriteData(famt.c_str(), famt.length());
    }
}

void Set_Lift_motor(double value)
{
   

    dc.litf = LimitingAmplitude(value, 1);
}
void Set_Acc_motor(double value)
{
    dc.acc = LimitingAmplitude(value, 1);
}
void Set_Turn_motor(double value)
{
    value=value*57.3/100;
    dc.turn = LimitingAmplitude(value, 1);
}
void Set_AUTO(bool value)
{
    dc.ao = LimitingAmplitude(value, 1);
}