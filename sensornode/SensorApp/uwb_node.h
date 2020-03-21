/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-21 13:29:51
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "sensornode.h"
#include<string.h>
class uwb_node : public Sensor_node
{
private:
    DRIVER_HANDLE _handle;
    std::string server_ip;
    int server_port;
    uint16_t source_id;
    void initdata();
    void updateData(uint32_t seq, time_t timestamp);
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);

public:
    _data::LOCATION_DATA _data;
    uwb_node(ProtocolAnalysis *msg);
    ~uwb_node();
};
