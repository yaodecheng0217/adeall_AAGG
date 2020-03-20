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

#ifndef _APP_H_
#define _APP_H_

using namespace msgpa;
using namespace adeall;
class APP
{
private:
    ProtocolAnalysis *_msg;
    std::string SM_ip = "127.0.0.1";
    int SM_prot = 9002;
     
    struct RES
    {
        uint32_t seq;
        int *code;
    };
    MutexLock rslist_lock;
    std::vector<RES> _respondlist;
    uint32_t _seq = 0;
    uint8_t timeout = 100;
void setCode(uint32_t ack, uint32_t seq);

public:
    DRIVER_HANDLE uwb_handle = {"uwb_sensor", 1, DATA_LIST::LOCATION};
    APP(ProtocolAnalysis *msg, int n) : _msg(msg) { uwb_handle.driver_id = n; };
    void run();
    void send();
    void _Callback(ReturnFrameData in);
    _data::LOCATION_DATA _data;
private:
    static void *hearbeat_thread(void *);
    void hearbeat_handle();
    void data_up_to_SM();
    void clear_sqe(uint32_t seq);
    int waitForACK(uint32_t seq,int * code,uint32_t timeout);
};
#endif