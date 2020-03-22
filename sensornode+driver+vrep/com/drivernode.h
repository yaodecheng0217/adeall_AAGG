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
    uint32_t _seq = 0;
    uint16_t _timeout = 50;
    uint16_t Frequency = 50;
    MutexLock time_lock;
   
public:
    Driver_node(ProtocolAnalysis *msg) : _msg(msg){};
    ~Driver_node(){};
    void run();
    void _Callback(ReturnFrameData in);
    void SetTimeout(uint16_t timeout);
    void SetFrequency(uint16_t Hz);

protected:
    DRIVER_HANDLE _handle;
    ProtocolAnalysis *_msg;
    virtual void initdata() = 0;
    virtual void sendData(uint32_t seq, time_t timestamp) = 0;
    virtual void sendHandle(uint32_t seq) = 0;
    virtual int setDoubleValue(uint16_t type, double value)=0;
private:
    static void *hearbeatThread(void *);
    static void *timer(void *);
    void sendHandleAction();
    void sendDataLoop();
    void clearSqe(uint32_t seq);
    int waitForACK(uint32_t seq, int *code, uint32_t timeout);
    void recvAckCode(uint32_t ack, uint32_t seq);
    void _Callback_Get(ReturnFrameData in);
    void _Callback_Set(ReturnFrameData in);
    void _Callback_ACK(ReturnFrameData in);
    void _Callback_HEARBEAT(ReturnFrameData in);
    void set_ACK_send(const char *ip, int prot, int code, uint32_t seq);
};
#endif