/*
 * @Description: In User Settings Edit
 * @Author: yaodecheng
 * @Date: 2019-10-16 13:09:28
 * @LastEditTime: 2019-10-16 13:17:37
 * @LastEditors: Please set LastEditors
 */

#ifndef _PID_H_
#define _PID_H_
typedef struct
{
	float Kp=0;                       //比例系数Proportional
	float Ki=0;                       //积分系数Integral
	float Kd=0;                       //微分系数Derivative

	float Ek=0;                       //当前误差
	float Ek1=0;                      //前一次误差 e(k-1)
	float Ek2=0;                      //再前一次误差 e(k-2)
}PID_IncTypeDef;

typedef struct
{
	float Kp=0;                       //比例系数Proportional
	float Ki=0;                       //积分系数Integral
	float Kd=0;                       //微分系数Derivative

	float Ek=0;                       //当前误差
	float Ek1=0;                      //前一次误差 e(k-1)
	float Ek2=0;                      //再前一次误差 e(k-2)
	float LocSum=0;                   //累计积分位置
}PID_LocTypeDef;


/************************************************
函数名称 ： PID_Loc
功    能 ： PID位置(Location)计算
参    数 ： SetValue ------ 设置值(期望值)
			ActualValue --- 实际值(反馈值)
			PID ----------- PID数据结构
返 回 值 ： PIDLoc -------- PID位置
*************************************************/
float PID_Loc(float SetValue, float ActualValue, PID_LocTypeDef* PID);
/************************************************
函数名称 ： PID_Inc
功    能 ： PID增量(Increment)计算
参    数 ： SetValue ------ 设置值(期望值)
			ActualValue --- 实际值(反馈值)
			PID ----------- PID数据结构
返 回 值 ： PIDInc -------- 本次PID增量(+/-)
*************************************************/
float PID_Inc(float SetValue, float ActualValue, PID_IncTypeDef* PID);

#endif
