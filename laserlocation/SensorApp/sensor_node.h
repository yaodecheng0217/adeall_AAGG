/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-30 10:41:07
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#ifndef _LASERSCA_APP_
#define _LASERSCA_APP_

#include "drivernode.h"
#include <string.h>

#define APP_name LaserSensor

class APP_name : public Driver_node
{
private:
    struct DATA
    {
        double x = 0;
        double y = 0;
        double yaw = 0;
    } _data;
    void initdata();
    int setDoubleValue(std::string type, double value);
    void datalist_up();

public:
    APP_name(ProtocolAnalysis *msg);
    void updata(double x, double y, double yaw);
    ~APP_name();
};
#endif