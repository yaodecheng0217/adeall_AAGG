/*
 * @Author: Yaodecheng
 * @Date: 2020-03-28 12:01:30
 * @LastEditTime: 2020-03-28 12:23:49
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include <winsock2.h>
#include "LaserInterface.h"
#include "math.h"
#include <string>
#include <iostream>
using namespace std;
#define PI 3.14159265358979323846
int xorflag = 0; //校验结果
const int MAX_BLOCK = 10000;
const int NET_BUF_NUM = 100;
const int NET_BUFCOL_NUM = 5000;
char g_cNetRecvBuf[NET_BUF_NUM][NET_BUFCOL_NUM];
typedef struct TagNetDataStruct
{
	char m_pcData[20000];
	int m_n32Len;
	int m_n32BufCnt;
	int m_n32Channel;
} NetDataStruct;
NetDataStruct g_sNetData;
StruReturnVal ReturnVal;
char ArrayAngle[7200] = {0};  //角度补偿数组
char ArrayEnergy[7200] = {0}; //能量值数组
bool f_AddAng = false;		  //是否有角度补偿标志位
bool f_AddEnergy = false;	  //是否有能量标志位
int g_n32MaxDis = 50000;	  //最大测距值
int g_n32NetRcvdPkgs = 0;
int g_n32CircleNo = 0;	   //圈号变量（变化的圈号值）
bool b_readfinish = false; //一帧数据有没有读到最后一包的标志
int g_n32NetRecvID = 0;	   //每一帧数据的ID
SOCKET sockClient;
HWND m_hReciWnd;
typedef struct TagStructNetBuf
{
	char m_acbuf[MAX_BLOCK * 10];
	unsigned int m_u32in;
	unsigned int m_u32out;
	unsigned int m_u32size;
} StructNetBuf;
StructNetBuf g_sNetBuf;
CRITICAL_SECTION g_netcs;
char acRecvDataBuf[MAX_BLOCK];
updateCallback _updateCallback;
void SetCallback(updateCallback cb)
{
    _updateCallback=cb;
}
DWORD WINAPI RecvThreadProc(LPVOID lpParam) // 接收线程处理函数
{
	InitializeCriticalSection(&g_netcs);
	int l_n32TcpErr = 0; //0正常，1粘帧，2断帧
	memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));
	int nRecvSize = 1;
	while (nRecvSize > 0)
	{
		EnterCriticalSection(&g_netcs);
		nRecvSize = recv(sockClient, acRecvDataBuf, MAX_BLOCK, 0);
		if (nRecvSize <= 0)
		{
			continue;
		}
		if ((DWORD)nRecvSize > MAX_BLOCK)
		{
			printf("nRecvSize > MAX_BLOCK");
			continue;
		}
		//-----------------------新解析------------------------------------------
		if ((g_sNetBuf.m_u32in + nRecvSize) >= MAX_BLOCK * 10)
		{
			printf("(g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10\n");
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf)); //大于最大缓存，原来的数全清0
			continue;
		}
		memcpy(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32in], acRecvDataBuf, nRecvSize * sizeof(char));
		g_sNetBuf.m_u32in += nRecvSize;
		if (g_sNetBuf.m_u32in >= MAX_BLOCK * 10)
		{
			printf("g_sNetBuf.m_u32in >= MAX_BLOCK\n");
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));
			break;
		}
		while (g_sNetBuf.m_u32out < g_sNetBuf.m_u32in)
		{
			if (((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0xff &&
				 (unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 1] == 0xaa))
			{
				//计算包长度
				unsigned int l_u32reallen = 0;
				if ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0x02)
				{
					l_u32reallen = ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 4] << 24) |
								   ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 5] << 16) |
								   ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 6] << 8) |
								   ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 7] << 0);
					l_u32reallen = l_u32reallen + 9;
				}
				else if ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0xff)
				{
					l_u32reallen = ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 3] << 0) |
								   ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 2] << 8);
					l_u32reallen = l_u32reallen + 4;
				}
				else
				{
					g_sNetBuf.m_u32out++;
					continue;
				}
				//判断包长度与实际缓存长度之间的关系
				if (l_u32reallen <= (g_sNetBuf.m_u32in - g_sNetBuf.m_u32out + 1))
				{
					OnNetRecv(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out], l_u32reallen);
					if (xorflag == 1) //校验失败
					{
						printf("校验 failed!\n");
						int i;
						for (i = 1; i < l_u32reallen; i++)
						{
							if ((g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 1] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 2] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 3] == 0x02) || (g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i] == 0xff && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 1] == 0xff))
							{
								g_sNetBuf.m_u32out += i;
								//memset(&g_sNetBuf,0,sizeof(gti_sNetBuf));
								printf("！！！连帧 failed!\n");
								xorflag = 0;
								break;
							}
						}
						if (i == l_u32reallen && xorflag == 1)
						{
							g_sNetBuf.m_u32out += l_u32reallen;
						}
						xorflag = 0;
					}
					else
					{
						g_sNetBuf.m_u32out += l_u32reallen;
					}
				}
				else if (l_u32reallen >= MAX_BLOCK)
				{
					printf("l_u32reallen err %d\n", l_u32reallen);
					memset(&g_sNetBuf, 0, sizeof(g_sNetBuf)); //大于最大缓存，原来的数全清0
					break;
				}
				else
				{
					break;
				}
			}
			else
			{
				g_sNetBuf.m_u32out++;
			}
		}
		if ((g_sNetBuf.m_u32in + nRecvSize) >= MAX_BLOCK * 10)
		{
			printf("！！！(g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10\n");
			printf("g_sNetBuf.m_u32in is %d,g_sNetBuf.m_u32out is%d\n", g_sNetBuf.m_u32in, g_sNetBuf.m_u32out);
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf)); //大于最大缓存，原来的数全清0
			continue;
		}
		if (g_sNetBuf.m_u32out >= g_sNetBuf.m_u32in)
		{
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));
		}
		LeaveCriticalSection(&g_netcs);
	}
	return 0;
}

bool ConnectServer(HWND hReciveWnd, NetConnectPara *NetStruct)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	int len = sizeof(int);
	struct timeval timeout = {3, 0}; //3s
	fd_set set;
	unsigned long ul = 1;
	bool ret = false;
	wVersionRequested = MAKEWORD(1, 1);			   //声明调用不同的Winsock版本。例如MAKEWORD(2,2)就是调用2.2版，MAKEWORD(1,1)就是调用1.1版。
	err = WSAStartup(wVersionRequested, &wsaData); //使用Socket的程序在使用Socket之前必须调用WSAStartup函数。以后应用程序就可以调用所请求的Socket库中的其它Socket函数了
	if (err != 0)
		return FALSE;

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion != 1))
	{
		WSACleanup();
		return FALSE;
	}
	// 创建客户端套接字
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET)
	{
		return FALSE;
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(NetStruct->IP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(NetStruct->Port);
	ioctlsocket(sockClient, FIONBIO, &ul);
	int iConnect = connect(sockClient, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	//若连接成功
	if (iConnect == -1)
	{
		FD_ZERO(&set); //将指定的文件描述符集清空，在对文件描述符集合进行设置前，必须对其进行初始化，如果不清空，由于在系统分配内存空间后，
		//通常并不作清空处理，所以结果是不可知的。
		FD_SET(sockClient, &set); //用于在文件描述符集合中增加一个新的文件描述符。
		if (select(sockClient + 1, NULL, &set, NULL, &timeout) > 0)
		{
			getsockopt(sockClient, SOL_SOCKET, SOCKET_ERROR, (char *)err, &len);
			if (err == 0)
			{
				ret = true;
			}
			else
			{
				ret = false;
			}
		}
		else
			ret = false;
	}
	else
		ret = false;
	ul = 0;
	ioctlsocket(sockClient, FIONBIO, &ul);
	if (!ret)
	{
		return FALSE;
	}
	NetStruct->ConnectState = true;
	m_hReciWnd = hReciveWnd;
	DWORD dwThreadID = 0;
	HANDLE handleFirst = CreateThread(NULL, 0, RecvThreadProc, 0, 0, &dwThreadID);
	return TRUE;
}

bool DisconnectServer(NetConnectPara *NetStruct)
{
	if (NetStruct->ConnectState = true)
	{
		Sleep(100);
		closesocket(sockClient);
		WSACleanup();
		NetStruct->ConnectState = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool checkXor(char *recvbuf, int recvlen)
{
	int i = 0;
	char check = 0;
	char *p = recvbuf;
	int len; //末尾两位无需校验
	if (*p == (char)0xff)
	{
		len = recvlen - 2;
		p++;
		p++;
		for (i = 0; i < len; i++)
		{
			check ^= *p;
			p++;
		}
		p++;
	}
	else if (*p == (char)0x02)
	{
		p = p + 8;
		len = recvlen - 9;
		for (i = 0; i < len; i++)
		{
			check ^= *p++;
		}
	}
	else
	{
		return false;
	}

	if (check == *p)
	{
		return true;
	}
	else
	{
		return false;
	}
}




int temp_Packnum = 0;
int PackSum = 0;
void OnNetRecv(char *pDataBuf, int nDataBufSize)
{
	if (nDataBufSize > 0)
	{

		if (checkXor(pDataBuf, nDataBufSize))
		{
			//printf("nDataBufSize=%d\n", nDataBufSize);
			memcpy(g_cNetRecvBuf[g_n32NetRecvID], pDataBuf, nDataBufSize);
			//printf("id=%d\n", (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][22]);

			switch ((unsigned char)g_cNetRecvBuf[g_n32NetRecvID][22])
			{
#pragma region 0x06
			case 0x06:
			{
				switch ((unsigned char)g_cNetRecvBuf[g_n32NetRecvID][23])

				{
				case 0x08:
				{
					uint8_t work_state = g_cNetRecvBuf[g_n32NetRecvID][26];
					uint8_t recvType = g_cNetRecvBuf[g_n32NetRecvID][27];
					/*
			              00：自身定位数据无效
                          01：连续定位
                          02：虚拟定位
                          03：初始定位
			*/
					uint8_t locationFlag = g_cNetRecvBuf[g_n32NetRecvID][28];
                    uint8_t datat[12];
					for (size_t i = 0; i < 12; i++)
					{
						datat[11-i]=g_cNetRecvBuf[g_n32NetRecvID][29+i];
					}
					 int32_t yaw,posx,posy;
					memcpy(&posy, &datat, 4);
					memcpy(&posx, &datat[4], 4);
					memcpy(&yaw, &datat[8], 4);

			
					uint8_t sacn_N = g_cNetRecvBuf[g_n32NetRecvID][41];
					uint8_t loc_N = g_cNetRecvBuf[g_n32NetRecvID][42];


                    uint8_t Temp[4]={0x4a ,0x79,0x05,0x00};//{0x00 ,0x05,0x79,0x4a};
					/*
					for (size_t i = 0; i < nDataBufSize; i++)
					{
						printf("%.2X ",(uint8_t)g_cNetRecvBuf[g_n32NetRecvID][i]);
					}
					printf("\n");*/
                   
					//std::cout<<yaw<<"  "<<posx<<"  "<<posy<<std::endl;
					//printf("work_state=%d  recvType=%d locationFlag=%d yaw=%.2f x=%d,y=%d,sacn_N=%d loc_N=%d\n", work_state, recvType, locationFlag, yaw/1000.0, posx, posy, sacn_N, loc_N);
                    if(_updateCallback!=NULL)
                    {
                        _updateCallback(posx,posy,yaw);
                    }
				}
				break;
				case 0x04:
				{
					if (g_cNetRecvBuf[g_n32NetRecvID][29] == 1)
						printf("设置导航模式完成！\n");
					else
					{
						printf("设置导航模式失败！\n");
					}
				}
				break;
				}
			}
			break;
#pragma region 0x02
			case 0x02:
			{

				g_n32NetRcvdPkgs++;
				int CirclNo = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][81]; //圈数
				if (g_n32NetRcvdPkgs == 1)										//如果是第一包数据
				{
					g_n32CircleNo = CirclNo; //记录第一包数据的圈号
				}
				int PackNo = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][82];	  //单圈包号
				int MarkScanData = (BYTE)g_cNetRecvBuf[g_n32NetRecvID][79];		  //扫描数据类型
				int RatioMark = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][80]; //分辨率标志
				int RightPackNum;												  //一帧正确的包数和
				switch (MarkScanData)
				{
				case 0: //距离
					if (RatioMark == 0)
					{
						RightPackNum = 78;
					}
					else if (RatioMark == 1)
					{
						RightPackNum = 21;
					}
					f_AddAng = false;
					f_AddEnergy = false;
					break;
				case 3: //距离+角度
					if (RatioMark == 0)
					{
						RightPackNum = 171;
					}
					else if (RatioMark == 1)
					{
						RightPackNum = 45;
					}
					f_AddAng = true;
					f_AddEnergy = false;
					break;
				case 4: //距离+能量
					if (RatioMark == 0)
					{
						RightPackNum = 171;
					}
					else if (RatioMark == 1)
					{
						RightPackNum = 45;
					}
					f_AddAng = false;
					f_AddEnergy = true;
					break;
				case 6: //距离+角度+能量
					if (RatioMark == 0)
					{
						RightPackNum = 300;
					}
					else if (RatioMark == 1)
					{
						RightPackNum = 78;
					}
					f_AddAng = true;
					f_AddEnergy = true;
					break;
				}
				if (RatioMark == 0) //12包
				{
					switch (PackNo)
					{
					case 1:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 2:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 3:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 4:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 5:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 6:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 7:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[7200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 8:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[8400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 9:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[9600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 10:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[10800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 11:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[12000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 12:
						PackSum += PackNo;
						if (MarkScanData == 0) //扫描数据类型为距离
						{
							temp_Packnum = PackSum;
							b_readfinish = true;
						}
						memcpy(&g_sNetData.m_pcData[13200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 13:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6) //距离+角度 或者距离+角度+能量
						{
							memcpy(&ArrayAngle[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4) //距离+能量
						{
							memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 14:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 15:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 16:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 17:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 18:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 4) //扫描数据类型为距离+角度或者距离+能量
						{
							temp_Packnum = PackSum;
							b_readfinish = true;
						}
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 19:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 20:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 21:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 22:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 23:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 24:
						PackSum += PackNo;
						if (MarkScanData == 6) //扫描数据类型为距离+角度+能量
						{
							temp_Packnum = PackSum;
							b_readfinish = true;
						}
						memcpy(&ArrayEnergy[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					default:
						break;
					}
				}
#pragma region 角度分辨率0 .1
				else if (RatioMark == 1)
				{
					switch (PackNo)
					{
					case 1:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 2:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 3:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 4:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 5:
						PackSum += PackNo;
						memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 6:
						PackSum += PackNo;
						if (MarkScanData == 0) //扫描数据类型为距离
						{
							temp_Packnum = PackSum;
							b_readfinish = true;
						}
						memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 7:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6) //距离+角度 或者距离+角度+能量
						{
							memcpy(&ArrayAngle[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4) //距离+能量
						{
							memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 8:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 9:
						PackSum += PackNo;
						if (MarkScanData == 3 || MarkScanData == 4) //扫描数据类型为距离+角度或者距离+能量
						{
							temp_Packnum = PackSum;
							b_readfinish = true;
						}
						if (MarkScanData == 3 || MarkScanData == 6)
						{
							memcpy(&ArrayAngle[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						else if (MarkScanData == 4)
						{
							memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						}
						break;
					case 10:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 11:
						PackSum += PackNo;
						memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					case 12:
						PackSum += PackNo;
						if (MarkScanData == 6) //扫描数据类型为距离+角度+能量
						{
							temp_Packnum = PackSum;
							b_readfinish = true;
						}
						memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
						break;
					default:
						break;
					}
				}
#pragma endregion
				if (g_n32CircleNo != CirclNo)
				{
					PackSum = 0;
					int BufLength = 0;
					float AngRatio = 0;
					if (RatioMark == 0)
					{
						BufLength = 14400;
						AngRatio = 0.05;
					}
					else if (RatioMark == 1)
					{
						BufLength = 7200;
						AngRatio = 0.1;
					}
					if (b_readfinish && temp_Packnum == RightPackNum)
					{
						ReturnVal.Ratio = RatioMark;
						b_readfinish = false;
						int nDataZhiCount = 0;
						int nDataJi = 0;
						float l_64nDataRegionZhiX = 0;
						float l_64nDataRegionZhiY = 0;
						float l_n64actualangle = 0;
						if (f_AddAng)
						{
							for (int i = 0; i < BufLength / 2; i++) //解析角度补偿值
							{
								ReturnVal.g_n64ArrayCompenAngle[i] = (unsigned char)(ArrayAngle[i]);
							}
						}
						if (f_AddEnergy)
						{
							for (int k = 0; k < BufLength / 2; k++) //解析能量值
							{
								ReturnVal.g_n64ArrayEnergy[k] = (unsigned char)(ArrayEnergy[k]);
							}
						}
						for (int m = 0; m < BufLength; m += 2)
						{
							nDataJi = ((g_sNetData.m_pcData[m] & 0xFF) << 8) + (BYTE)g_sNetData.m_pcData[m + 1];
							if (nDataJi > g_n32MaxDis) //最大测距值
								nDataJi = g_n32MaxDis;
							l_n64actualangle = nDataZhiCount * AngRatio + ReturnVal.g_n64ArrayCompenAngle[nDataZhiCount] / 100.0;
							l_64nDataRegionZhiX = nDataJi * cos(l_n64actualangle / 180 * 3.14) / 1000.0;
							l_64nDataRegionZhiY = nDataJi * sin(l_n64actualangle / 180 * 3.14) / 1000.0;
							ReturnVal.g_n64ArrayZhiX[nDataZhiCount] = l_64nDataRegionZhiX;
							ReturnVal.g_n64ArrayZhiY[nDataZhiCount] = l_64nDataRegionZhiY;
							if (nDataZhiCount == 1)
								printf("cont = %d, x=%f  y=%f   yaw=%f\n", nDataZhiCount, l_64nDataRegionZhiX, l_64nDataRegionZhiY, l_n64actualangle);
							nDataZhiCount++;
						}
						SendMessage(m_hReciWnd, WM_SERVER_SendData, (WPARAM)&ReturnVal, 0);
					}
					temp_Packnum = 0;
					memset(&g_sNetData, 0, sizeof(g_sNetData));
					g_n32CircleNo = CirclNo;
				}
			}
			break;
#pragma endregion
			default:
				break;
			}
			g_n32NetRecvID = (g_n32NetRecvID + 1) % NET_BUF_NUM;
		}
		else
		{
			xorflag = 1;
		}
	}
}

void SendData(NetConnectPara *NetStruct)
{
	if (NetStruct->Len_Senddata <= 0)
	{
		return;
	}
	//已发送字节数
	int iSentBytes = 0;
	//未发送字节数;
	int iRemain = NetStruct->Len_Senddata;
	//发送起始位置
	int iPos = 0;
	while (iRemain > 0)
	{
		iSentBytes = send(sockClient, &NetStruct->Senddata[iPos], iRemain, 0);
		if (iSentBytes == iRemain)
		{
			//全部发送完成
			break;
		}
		if (iSentBytes == SOCKET_ERROR)
		{
			return;
		}
		iRemain -= iSentBytes;
		iPos += iSentBytes;
	}
}
