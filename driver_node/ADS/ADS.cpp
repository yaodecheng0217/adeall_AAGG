#include "ADS.h"

ADS car_ads;


void ADS::ADS_init(unsigned char netid[6])
{
	nPort = AdsPortOpen(); //打开ADS通讯端口

	pAddr->netId.b[0] = netid[0];       //手动填写目标设备的AMSNETID，比如要通信的目标设备AMSNETID为
	pAddr->netId.b[1] = netid[1];
	pAddr->netId.b[2] = netid[2];
	pAddr->netId.b[3] = netid[3];
	pAddr->netId.b[4] = netid[4];
	pAddr->netId.b[5] = netid[5];

	pAddr->port = 851; //将指针指向801端口,twincat3 是851端口

	init = true;
}

void ADS::ADS_init()
{
	nPort = AdsPortOpen(); //打开ADS通讯端口

	pAddr->netId.b[0] = netid[0];       //手动填写目标设备的AMSNETID
	pAddr->netId.b[1] = netid[1];
	pAddr->netId.b[2] = netid[2];
	pAddr->netId.b[3] = netid[3];
	pAddr->netId.b[4] = netid[4];
	pAddr->netId.b[5] = netid[5];

	pAddr->port = 851; //将指针指向801端口,twincat3 是851端口

	init = true;
}



ADS::ADS()
{
	InitializeCriticalSection(&cs);
	//ADS_init(netid);
}


ADS::~ADS()
{
	//logy << "ads~()" << std::endl;
}



void  ADS::send_INT(unsigned long addr_offset, int temp)
{
	if (!init)
		return;
	//EnterCriticalSection(&cs);
	while (AdsSyncWriteReq(pAddr, 0x4020, addr_offset * 4, 0x4, &temp)) {};
	//LeaveCriticalSection(&cs);

}
void  ADS::send_BOOL(unsigned long addr_offset, bool temp)
{
	if (!init)
		return;
	//EnterCriticalSection(&cs);
	while(AdsSyncWriteReq(pAddr, 0x4020, addr_offset, 0x1, &temp));
	//LeaveCriticalSection(&cs);
	//if (nErr) cerr << "Error: AdsSyncReadReq: " << nErr << '\n';

}

void  ADS::send_outputINT(unsigned long addr_offset, int temp)
{
	if (!init)
		return;
	//EnterCriticalSection(&cs);
	while(AdsSyncWriteReq(pAddr, 0xf030, 0x3E800 + addr_offset * 4, 0x4, &temp));
	//if (nErr) cerr << "Error: AdsSyncReadReq: " << nErr << '\n';
	//LeaveCriticalSection(&cs);

}
void  ADS::send_outputBOOL(unsigned long addr_offset, bool temp)
{
	if (!init)
		return;
	EnterCriticalSection(&cs);
	while(AdsSyncWriteReq(pAddr, 0xf030, 0x3E800 + addr_offset, 0x1, &temp));
	//if (nErr) cerr << "Error: AdsSyncReadReq: " << nErr << '\n';
	LeaveCriticalSection(&cs);

}
bool  ADS::read_outputBOOL(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	bool temp;
	while(AdsSyncReadReq(pAddr, 0xf030, 0x3E800 + addr_offset, 0x1, &temp));
	//LeaveCriticalSection(&cs);
	return temp;
	//if (nErr) cerr << "Error: AdsSyncReadReq: " << nErr << '\n';

}
bool  ADS::read_outputINT(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	bool temp;
	while(AdsSyncReadReq(pAddr, 0xf030, 0x3E800 + addr_offset * 4, 0x4, &temp));
	//LeaveCriticalSection(&cs);
	//if (nErr) cerr << "Error: AdsSyncReadReq: " << nErr << '\n';
   return 1;
}

int  ADS::read_INT(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	int temp;
	while(AdsSyncReadReq(pAddr, 0x4020, addr_offset * 4, 0x4, &temp));
	//LeaveCriticalSection(&cs);
	//if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';
	return temp;

}
int  ADS::read_inputINT(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	int temp;
	while(AdsSyncReadReq(pAddr, 0xF020, 0x1F400 + addr_offset * 4, 0x4, &temp));
	//if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';
	//LeaveCriticalSection(&cs);
	return temp;

}
bool  ADS::read_BOOL(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	bool temp;
	while(AdsSyncReadReq(pAddr, 0x4020, addr_offset, 0x1, &temp));
	//if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';
	//LeaveCriticalSection(&cs);
	return temp;

}
bool  ADS::read_inputBOOL(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	bool temp;
	while(AdsSyncReadReq(pAddr, 0xF020, 0x1F400 + addr_offset, 0x1, &temp));
	//if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';
	//LeaveCriticalSection(&cs);
	return temp;

}

float  ADS::read_float(unsigned long addr_offset)
{
	if (!init)
		return 0;
	//EnterCriticalSection(&cs);
	float temp;
	while(AdsSyncReadReq(pAddr, 0x4020, addr_offset*4, 0x4, &temp));
	//if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';
	//LeaveCriticalSection(&cs);
	return temp;

}

void  ADS::send_float(unsigned long addr_offset, float temp)
{	
	if (!init)
		return;
	EnterCriticalSection(&cs);
	while(AdsSyncWriteReq(pAddr, 0x4020, addr_offset * 4, 0x4, &temp));
	LeaveCriticalSection(&cs);
}

void ADS::NotificationReq_MD(unsigned long addr_offset, PAdsNotificationFuncEx callback,void * hUser)
{
	ULONG                  hNotification;
	AdsNotificationAttrib  adsNotificationAttrib;

	//设置notification的属性值
	adsNotificationAttrib.cbLength = 4;
	adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	adsNotificationAttrib.nMaxDelay = 20000; // 2sec 
	adsNotificationAttrib.nCycleTime = 10000;//10000000; // 1sec 
	// 32-Bit变量(和指针),这些变量(和指针)可以被传递给callback-function 
	//hUser = 3474573467;
	//开始PLC变量的传输
	nErr = AdsSyncAddDeviceNotificationReq(pAddr, 0x4020, addr_offset * 4, &adsNotificationAttrib, callback, (ULONG)hUser, &hNotification);
}
void ADS::NotificationReq_MB(unsigned long addr_offset, PAdsNotificationFuncEx callback)
{
	ULONG                  hNotification, hUser;
	AdsNotificationAttrib  adsNotificationAttrib;

	//设置notification的属性值
	adsNotificationAttrib.cbLength = 1;
	adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	adsNotificationAttrib.nMaxDelay = 200; // 2sec 
	adsNotificationAttrib.nCycleTime = 100;//10000000; // 1sec 
	// 32-Bit变量(和指针),这些变量(和指针)可以被传递给callback-function 
	hUser = 3474573467;
	//开始PLC变量的传输
	nErr = AdsSyncAddDeviceNotificationReq(pAddr, 0x4020, addr_offset, &adsNotificationAttrib, callback, hUser, &hNotification);
}
void ADS::NotificationReq_ID(unsigned long addr_offset, PAdsNotificationFuncEx callback)
{
	ULONG                  hNotification, hUser;
	AdsNotificationAttrib  adsNotificationAttrib;

	//设置notification的属性值
	adsNotificationAttrib.cbLength = 4;
	adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	adsNotificationAttrib.nMaxDelay = 20000; // 2sec 
	adsNotificationAttrib.nCycleTime = 10000;//10000000; // 1sec 
	// 32-Bit变量(和指针),这些变量(和指针)可以被传递给callback-function 
	hUser = 3474573467;
	//开始PLC变量的传输
	nErr = AdsSyncAddDeviceNotificationReq(pAddr, 0xF020, 0x1F400 + addr_offset * 4, &adsNotificationAttrib, callback, hUser, &hNotification);
}
void ADS::NotificationReq_IB(unsigned long addr_offset, PAdsNotificationFuncEx callback)
{
	ULONG                  hNotification, hUser;
	AdsNotificationAttrib  adsNotificationAttrib;

	//设置notification的属性值
	adsNotificationAttrib.cbLength = 1;
	adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	adsNotificationAttrib.nMaxDelay = 20; // 2sec 
	adsNotificationAttrib.nCycleTime = 10;//10000000; // 1sec 
	// 32-Bit变量(和指针),这些变量(和指针)可以被传递给callback-function 
	hUser = 3474573467;
	//开始PLC变量的传输
	nErr = AdsSyncAddDeviceNotificationReq(pAddr, 0xF020, 0x1F400 + addr_offset, &adsNotificationAttrib, callback, hUser, &hNotification);
}