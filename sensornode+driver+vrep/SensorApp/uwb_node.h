/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-22 16:50:20
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "../com/drivernode.h"
#include<string.h>
class uwb_node : public Driver_node
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
    LOCATION_DATA _data;
    uwb_node(ProtocolAnalysis *msg);
    ~uwb_node();
};
