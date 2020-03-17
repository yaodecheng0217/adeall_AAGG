/*
 * @Description: In User Settings Edit
 * @Author:yaodecheng
 * @Date: 2019-10-16 13:08:22
 * @LastEditTime: 2019-10-16 13:11:37
 * @LastEditors: Please set LastEditors
 */
#include"PID.h"


/************************************************
函数名称 ： PID_Inc
功    能 ： PID增量(Increment)计算
参    数 ： SetValue ------ 设置值(期望值)
			ActualValue --- 实际值(反馈值)
			PID ----------- PID数据结构
返 回 值 ： PIDInc -------- 本次PID增量(+/-)
*************************************************/
float PID_Inc(float SetValue, float ActualValue, PID_IncTypeDef* PID)
{
	float PIDInc;                                  //增量

	PID->Ek = SetValue - ActualValue;
	PIDInc = (PID->Kp * PID->Ek) - (PID->Ki * PID->Ek1) + (PID->Kd * PID->Ek2);

	PID->Ek2 = PID->Ek1;
	PID->Ek1 = PID->Ek;  
	return PIDInc;
}


/************************************************
函数名称 ： PID_Loc
功    能 ： PID位置(Location)计算
参    数 ： SetValue ------ 设置值(期望值)
			ActualValue --- 实际值(反馈值)
			PID ----------- PID数据结构
返 回 值 ： PIDLoc -------- PID位置
*************************************************/
float PID_Loc(float SetValue, float ActualValue, PID_LocTypeDef* PID)
{
	float PIDLoc;                                  //位置

	PID->Ek = SetValue - ActualValue;
	PID->LocSum += PID->Ek;                         //累计误差

	PIDLoc = PID->Kp * PID->Ek + (PID->Ki * PID->LocSum) + PID->Kd * (PID->Ek1 - PID->Ek);

	PID->Ek1 = PID->Ek;  return PIDLoc;
}