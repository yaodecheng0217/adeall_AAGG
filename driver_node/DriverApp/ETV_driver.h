/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-29 23:00:57
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#ifndef _DRIVER_APP_
#define _DRIVER_APP_

#include "drivernode.h"
#include <string.h>

#define APP_name Car_control

class APP_name : public Driver_node
{
private:
    struct DATA
    {
        double acc = 0;
        double turn = 0;
        double lift = 0;
        bool auto_mode = 0;
    }_data;

    void initdata();
    int setDoubleValue(std::string type, double value);
    static void *controlOnline(void *etv);
    int Control_count=0;
public:
    APP_name(ProtocolAnalysis *msg);
    ~APP_name();
};
#endif