/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-28 12:07:29
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "drivernode.h"
#include<string.h>

#define APP_name LaserSensor

class APP_name : public Driver_node
{
private:
    void initdata();
    int setDoubleValue(uint16_t type, double value);
public:
    LOCATION_DATA _data;
    APP_name(ProtocolAnalysis *msg);
    void updateX(double x);
    void updatey(double y);
    void updateyaw(double yaw);
    ~APP_name();
};