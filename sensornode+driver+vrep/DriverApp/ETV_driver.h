/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-24 10:58:24
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "drivernode.h"
#include "../vrep/vrep_interface.h"

class ETV_driver : public Driver_node
{
private:
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
    vrep_interface *vr;
    ETV_driver(ProtocolAnalysis *msg,int n,vrep_interface *vrep);
    ~ETV_driver();
};


