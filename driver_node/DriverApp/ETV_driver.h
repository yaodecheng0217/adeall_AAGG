/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-23 14:21:02
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "drivernode.h"

class ETV_driver : public Driver_node
{
public:
    std::string server_ip;
    int server_port;
    uint16_t source_id;
    void initdata();
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);
    int setDoubleValue(uint16_t type, double value);
    uint16_t Control_count=10;
    static void *controlOnline(void *);
public:
    ETV_DRIVER_STATE_DATA _data;
    ETV_driver(ProtocolAnalysis *msg,int n);
    ~ETV_driver();
};


