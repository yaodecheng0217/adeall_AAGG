
/*
 * @Description: In User Settings Edit
 * @Author: Yaodecheng
 * @Date: 2019-10-13 14:35:32
 * @LastEditTime: 2020-03-24 10:44:38
 * @LastEditors: Yaodecheng
 */

#include "UdpMessage.h"
#include"thread_base.h"
#define logi(_fmt,...) printf("[I] ");printf(_fmt "\n", ##__VA_ARGS__)
#define loge(_fmt,...) printf("[E] ");printf(_fmt "\n", ##__VA_ARGS__)
UdpMessage::UdpMessage()
{
     
}

UdpMessage::~UdpMessage()
{
}
//单独创建一帧数据处理线程
void *UdpMessage::tRecv_thread(void *prt)
{
    UdpMessage *p = (UdpMessage *)prt;
    std::vector<uint8_t> buffer;
    buffer.resize(p->recvL);   

    for (size_t i = 0; i < p->recvL; i++)
    {
       buffer[i]=p->recv_buf[i];
    }
    p->recvcs.unlock();
    p->CallBackFuntion(buffer, p->_context); 
    return 0;
}


void *UdpMessage::UDPrevthreadfun(void *ptr)
{
    UdpMessage *P = (UdpMessage *)ptr;
    //struct sockaddr_in addr_client;
    int len = sizeof(P->addr);
    while (1)
    {
        P->recvcs.lock();//转移缓冲区数据前上锁
#ifdef _WIN32
        P->recvL = recvfrom(P->sock_fd, P->recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&P->addr_client, &len);
#else
        P->recvL = recvfrom(P->sock_fd, P->recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&P->addr_client, (socklen_t *)&len);
#endif
        if (P->recvL <= 0)
        {
             P->recvcs.unlock();//转移缓冲区数据前上锁
            continue;
           
        }
        else
        {
            //printf(">>>>%d,%s\n",ntohs(P->addr_client.sin_port),inet_ntoa(P->addr_client.sin_addr));
            thread_base t(P->tRecv_thread,P);
            //tRecv_thread(P);
        }
    }
    return 0;
}

int UdpMessage::init(int cliceport, void *prt)
{
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    int err;
    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        return -1;
    }

    if (LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        return -1;
    }
#endif

    port = cliceport;
    _context = prt;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        loge("udp %d socket error!\n", cliceport);
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
#ifdef _WIN32
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

    int len = sizeof(addr);
    if (bind(sock_fd, (sockaddr *)&addr, len) == -1)
    {
        loge("udp %d bink error!\n", cliceport);
#ifdef _WIN32
        closesocket(sock_fd);
#else
        close(sock_fd);
#endif
        return -1;
    }
    else
    {
        logi("udp %d bink ok!\n", cliceport);
        thread_base t2(UDPrevthreadfun,this);
    }

    return 0;
}

int UdpMessage::messagsend(const char *ip, int port, const char *data, int L)
{
    //ScopeLocker lock(&sendcs);
    struct sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port);
#ifdef _WIN32
    addr_server.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    addr_server.sin_addr.s_addr = inet_addr(ip);
#endif

    int len = sizeof(addr_server);
    int rc=sendto(sock_fd, data, L, 0, (struct sockaddr *)&addr_server, len);
    if ( rc< 0)
    {
        logi("udp ip=%s port=%d send error!%d\n", ip, port, sock_fd);
    }
    else
    {
        //logi("udp ip=%s port=%d send ok ! %d\n", ip, port,sock_fd);
    }
    return rc;
}
int UdpMessage::messagsend(const char *ip, int port, const std::string data)
{
    return messagsend(ip, port, data.c_str(), data.length());
}
