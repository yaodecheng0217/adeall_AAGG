/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:15
 * @LastEditors: Yaodecheng
 */
#include "ProtocolAnalysis.h"
#include "CJson/CJsonObject.hpp"
#include <string>
#include "define_msg_data.h"
#include <vector>

//传感器类节点
//1.更新数据，掉线检测，
//2.应答机制
//3.固定频率发送数据

#ifndef _DRIVER_NODE_H_
#define _DRIVER_NODE_H_

using namespace msgpa;
using namespace adeall;
static uint32_t _seq = 0;
class Driver_node
{
private:
    struct RES
    {
        uint32_t seq;
        int *code;
    };
    MutexLock _rslist_lock;
    std::vector<RES> _respondlist;

    uint16_t _timeout = 3;
    uint16_t _SendingInterval = 20;
    MutexLock _time_lock;
    int _cnt = 10;

public:
    Driver_node(ProtocolAnalysis *msg) : _msg(msg){};
    ~Driver_node(){};
    void run();
    void _Callback(ReturnFrameData in);
    void SetTimeout(uint16_t timeout);
    void SetSendingInterval(uint16_t Hz);
    void printfNodeInfo();

protected:
    std::string server_ip;
    int server_port;
    uint16_t source_id;
    DRIVER_HANDLE _handle;
    ProtocolAnalysis *_msg;
    virtual void initdata() = 0;
    virtual void datalist_up() = 0;
    virtual int setDoubleValue(std::string type, double value) = 0;

private:
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);
    static void *hearbeatThread(void *);
    static void *timer(void *);
    int sendHandleAction();
    void sendDataLoop();
    void clearSqe(uint32_t seq);
    int waitForACK(uint32_t seq, int *code, uint32_t timeout);
    void recvAckCode(int ack, uint32_t seq);
    void _Callback_Get(ReturnFrameData in);
    void _Callback_Set(ReturnFrameData in);
    void _Callback_ACK(ReturnFrameData in);
    void _Callback_HEARBEAT(ReturnFrameData in);
    void set_ACK_send(const char *ip, int prot, int code, uint32_t seq);
};
#endif