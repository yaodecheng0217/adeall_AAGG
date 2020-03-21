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

#ifndef _APP_H_
#define _APP_H_

using namespace msgpa;
using namespace adeall;
class Sensor_node
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
    Sensor_node(ProtocolAnalysis *msg) : _msg(msg){};
    ~Sensor_node(){};
    void run();
    void _Callback(ReturnFrameData in);
    void SetTimeout(uint16_t timeout);
    void SetFrequency(uint16_t Hz);

protected:
    ProtocolAnalysis *_msg;
    virtual void initdata() = 0;
    virtual void sendData(uint32_t seq, time_t timestamp) = 0;
    virtual void sendHandle(uint32_t seq) = 0;
private:
    static void *hearbeatThread(void *);
    static void *timer(void *);
    void sendHandleAction();
    void sendDataLoop();
    void clearSqe(uint32_t seq);
    int waitForACK(uint32_t seq, int *code, uint32_t timeout);
    void recvAckCode(uint32_t ack, uint32_t seq);
};
#endif