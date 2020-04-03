/*
 * @Author: Yaodecheng
 * @Date: 2020-04-03 14:52:50
 * @LastEditTime: 2020-04-03 15:55:52
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#pragma once
#include <iostream>
#include <Windows.h>
#include<minwindef.h>
#include<TcAdsDef.h>
#include<TcAdsAPI.h>


class ADS
{

private:

	long      nErr, nPort;	//定义端口变量
	AmsAddr   Addr;		//定义AMS地址变量
	PAmsAddr  pAddr = &Addr;  	//定义端口地址变量
	bool init = false;
	//默认车辆ads地址  5.61.118.158.1.1
	unsigned char netid[6] = { 5,61,118,158,1,1 };
	CRITICAL_SECTION cs;
public:
	ADS();
	~ADS();

	void ADS_init(unsigned char netid[6]);
	void ADS_init();
	void send_INT(unsigned long addr_offset, int temp);
	void send_BOOL(unsigned long addr_offset, bool temp);
	int read_inputINT(unsigned long addr_offset);
	int read_INT(unsigned long addr_offset);
	bool read_BOOL(unsigned long addr_offset);
	bool read_inputBOOL(unsigned long addr_offset);
	void send_outputINT(unsigned long addr_offset, int temp);
	void send_outputBOOL(unsigned long addr_offset, bool temp);
	bool read_outputBOOL(unsigned long addr_offset);
	bool read_outputINT(unsigned long addr_offset);
	float read_float(unsigned long addr_offset);

	void send_float(unsigned long addr_offset, float temp);
	void NotificationReq_MD(unsigned long addr_offset, PAdsNotificationFuncEx callback,void * hUser);
	void NotificationReq_MB(unsigned long addr_offset, PAdsNotificationFuncEx callback);
	void NotificationReq_ID(unsigned long addr_offset, PAdsNotificationFuncEx callback);
	void NotificationReq_IB(unsigned long addr_offset, PAdsNotificationFuncEx callback);
	bool adsinitstate()
	{
		return init;
	}
};

extern unsigned char netid[6];
extern ADS car_ads;