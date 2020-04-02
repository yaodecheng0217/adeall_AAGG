/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-30 09:26:55
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#ifndef _UWB_APP_
#define _UWB_APP_

#include "drivernode.h"
#include <string.h>
class uwb_node : public Driver_node
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
    void updata(double x, double y, double yaw);
    uwb_node(ProtocolAnalysis *msg);
    ~uwb_node();
};

#endif