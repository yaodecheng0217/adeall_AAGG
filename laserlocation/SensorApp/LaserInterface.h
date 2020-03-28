/*
 * @Author: Yaodecheng
 * @Date: 2020-03-28 12:00:30
 * @LastEditTime: 2020-03-28 12:19:36
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include<windows.h>
#define WM_SERVER_SendData WM_USER + 162
struct NetConnectPara
{
	char *Senddata;
	int Len_Senddata;
	DWORD IP;
	int Port;
	bool ConnectState = false;
};
typedef struct ReturnVal
{
	int g_n64ArrayCompenAngle[7200];//角度补偿数组
	int g_n64ArrayEnergy[7200];//能量值数组
	double g_n64ArrayZhiX[7200];
	double g_n64ArrayZhiY[7200];
	double g_n64ArrayZhiZ[7200];
	int Ratio;//分辨率 0:0.05 1:0.1
} StruReturnVal;
void OnNetRecv(char *pDataBuf, int nDataBufSize); bool checkXor(char* recvbuf, int recvlen);
bool ConnectServer(HWND hReciveWnd, NetConnectPara*NetStruct);
bool DisconnectServer(NetConnectPara*NetStruct);
void SendData(NetConnectPara*NetStruct);

typedef void (*updateCallback)(INT32 x,INT32 y,INT32 yaw);


void SetCallback(updateCallback cb);