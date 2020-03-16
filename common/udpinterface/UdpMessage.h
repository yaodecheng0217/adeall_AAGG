/*
 * @Description: In User Settings Edit
 * @Author: Yaodecheng
 * @Date: 2019-10-13 14:02:01
 * @LastEditTime: 2020-03-16 14:06:01
 * @LastEditors: Yaodecheng
 */
/*

 // udp信息处理函数每收到一帧信息创建一个线程去处理

void *DataProcess(std::vector<uint8_t> databuffer,void * context)
{
	return 0;
}

int main()
{
	UdpMessage udptest;
	udptest.init(6666, DataProcess, NULL);
	udptest.messagsend("127.0.0.1", 7777, "123");
	while(1);
}

*/

#ifndef __UDPMESSAGE__
#define __UDPMESSAGE__
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "mutexlock.hpp"
#include <iostream>

class UdpMessage
{
public:
	UdpMessage();
	int init(int cliceport, void *);
	~UdpMessage();
	int messagsend(const char *ip, int port, const char *data, int L);
	int messagsend(const char *ip, int port, const std::string data);

private:

    static void *UDPrevthreadfun(void *);
    static void *tRecv_thread(void *);


	void recvudpfun();
	void *_context;
	int port;
	int sock_fd;
	struct sockaddr_in addr;
	MutexLock recvcs,sendcs;
	char recv_buf[2000];
	int recvL = 0;
   
protected:
    virtual void CallBackFuntion(std::vector<uint8_t>,void *) = 0;
	struct sockaddr_in addr_client;
};
#endif
