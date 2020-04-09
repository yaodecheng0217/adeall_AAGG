/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-04-09 15:09:47
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "uwb_node.h"

uwb_node::uwb_node(ProtocolAnalysis *msg) : Driver_node(msg)
{
}
uwb_node::~uwb_node()
{
}
void uwb_node::initdata()
{
    source_id = ID_Sensor_uwb;
   _handle.driver_name = "location";
   _handle.driver_id = 1;
   //??????
   _handle.data_list.Add("x", _data.x);
   _handle.data_list.Add("y", _data.y);
   _handle.data_list.Add("yaw", _data.yaw);
}
void uwb_node::datalist_up()
{
   _handle.data_list.Replace("x", _data.x);
   _handle.data_list.Replace("y", _data.y);
   _handle.data_list.Replace("yaw", _data.yaw);
}
void uwb_node::updata(double x, double y, double yaw)
{
   _data.x = x;
   _data.y = y;
   _data.yaw = yaw;
}
int uwb_node::setDoubleValue(std::string type, double value)
{
   return 0;
}