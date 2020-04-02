/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-28 11:35:19
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "drivernode.h"
#include<string.h>
class uwb_node : public Driver_node
{
private:
    void initdata();
    int setDoubleValue(uint16_t type, double value);
public:
    LOCATION_DATA _data;
    uwb_node(ProtocolAnalysis *msg);
    void updateX(double x);
    void updatey(double y);
    void updatez(double z);
    ~uwb_node();
};