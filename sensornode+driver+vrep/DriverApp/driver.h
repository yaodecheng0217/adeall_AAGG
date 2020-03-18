/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:15
 * @LastEditors: Yaodecheng
 */
#include "udpinterface/ProtocolAnalysis.h"
#include "CJson/CJsonObject.hpp"
#include <string>
#include "define_msg_data.h"
#include <vector>
#include "../vrep/vrep_interface.h"
#ifndef _DRIVER_H_
#define _DRIVER_H_

using namespace msgpa;
using namespace adeall;
class DRIVER
{
private:
    ProtocolAnalysis *_msg;
    vrep_interface *vr;
    std::string SM_ip = "127.0.0.1";
     
    struct RES
    {
        uint32_t seq;
        int *code;
    };
    std::vector<RES> _respondlist;
    uint32_t _seq = 0;
    uint8_t timeout = 100;
    _data::ETV_DRIVER_STATE_DATA _driver_state_data;
public:
    DRIVER_HANDLE Driver_handle = {"Driver", 1, DATA_LIST::ETV_DriverState};
    DRIVER(ProtocolAnalysis *msg, int n,vrep_interface *vrep) : _msg(msg),vr(vrep) { Driver_handle.driver_id = n; };
    void run();
    void send();
    void _Callback(ReturnFrameData in);
    
private:
    static void *hearbeat_thread(void *);
    void hearbeat_loop();
    void clear_sqe(uint32_t seq);
    void setCode(uint32_t ack,uint32_t seq);
    int setDoubleValue(uint8_t type,double value);
    int setBoolValue(uint8_t type,bool value);
    void set_ACK_send(const char *ip, int prot,int code, uint32_t seq);
};
#endif