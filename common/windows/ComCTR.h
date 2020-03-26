#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

typedef void (*dataFun)(int L, string str, void *);

class ComCTR
{
private:
	const char *com;

	HANDLE comfd;

	CRITICAL_SECTION m_csCommunicationSync;

	bool init = false;
	dataFun dataproces;
	void *up;
    bool testmode = false;
	string recvString = "";
	
	static DWORD WINAPI  Comrecv(void* args);
	static DWORD WINAPI  Comrecv2(void* args);
public:
	int Recv_length = 0;
	ComCTR();
	void Open(const char *szStr, int BaudRate, char endstr);
	void Open(const char *szStr, int BaudRate, char endstr, char endstr2);
	void Open(const char *szStr, int BaudRate);
	void testmodeOn();
	bool WriteData(const char *pData, unsigned int length);
	bool ReadChar(char &cRecved);
	string read();
	void setCallBack(dataFun, void *);
	bool comstate();
private:
	HANDLE Serialinit(const char *szStr, int BaudRate);
	char endF = '\n';
	char endF2 = '\n';

	
};
