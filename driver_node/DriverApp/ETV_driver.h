/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-21 14:23:07
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "drivernode.h"
class ETV_driver : private Driver_node
{
private:
    std::string server_ip;
    int server_port;
    uint16_t source_id;
    void initdata();
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);
    int setDoubleValue(uint16_t type, double value);

public:
    _data::ETV_DRIVER_STATE_DATA _data;
    ETV_driver(ProtocolAnalysis *msg);
    ~ETV_driver();
};


