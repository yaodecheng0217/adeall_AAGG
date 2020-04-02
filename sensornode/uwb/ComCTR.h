#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>


using namespace std;

//类型重定义：函数指针类型
typedef void (*dataFun)(int L,string str,void *);
	
class ComCTR
{
private:


	const char* com;//com口字符串

	HANDLE comfd;
	/** 同步互斥,临界区保护 */
	CRITICAL_SECTION   m_csCommunicationSync;       //!< 互斥操作串口  

	bool init = false;
	dataFun dataproces;
	void* up;
public:
	bool testmode = false;
	string recvString = "";
	int Recv_length = 0;
	 ComCTR();
	
	 void Open(const char* szStr, int BaudRate, char endstr);
	 void Open(const char* szStr, int BaudRate, char endstr, char endstr2);
	 void Open(const char* szStr, int BaudRate);
	 void testmodeOn();
	 HANDLE Serialinit(const char* szStr, int BaudRate);

	 bool WriteData(unsigned char* pData, unsigned int length);
	 bool ReadChar(char& cRecved);
	 string read();
	 void setCallBack(dataFun,void*);
	 char endF = '\n';
	 char endF2 = '\n';

	 bool comstate();


};

