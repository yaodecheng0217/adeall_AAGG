/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-30 09:33:48
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#ifndef _DRIVER_APP_
#define _DRIVER_APP_
#include "drivernode.h"
#include "../vrep/vrep_interface.h"

class ETV_driver : public Driver_node
{
private:
    struct DATA
    {
        double acc = 0;
        double turn = 0;
        double lift = 0;
        double side = 0;
        double tift =0;
        double forwarld=0;
        bool auto_mode = 0;
    }_data;
    void initdata();
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);
    int setDoubleValue(std::string type, double value);
    uint16_t Control_count=10;
    static void *controlOnline(void *);
    void datalist_up();
public:
    vrep_interface *vr;
    ETV_driver(ProtocolAnalysis *msg,int n,vrep_interface *vrep);
    ~ETV_driver();
};

#endif
