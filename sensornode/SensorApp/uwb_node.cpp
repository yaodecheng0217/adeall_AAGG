/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-04-02 18:00:16
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
   server_ip = "192.168.2.16";
   server_port = StateMachine_port;
   source_id = ID_Sensor_uwb;

   _handle.driver_name = "uwb_sensor";
   _handle.driver_id = 1;
   //添加维护数据
   _handle.data_list.Add("x", _data.x);
   _handle.data_list.Add("y", _data.y);
   _handle.data_list.Add("z", _data.z);
}
void uwb_node::datalist_up()
{
   _handle.data_list.Replace("x", _data.x);
   _handle.data_list.Replace("y", _data.y);
   _handle.data_list.Replace("z", _data.z);
}
void uwb_node::updateX(double x)
{
   _handle.data_list.Replace("x", x);
}
void uwb_node::updatey(double y)
{
   _handle.data_list.Replace("y", y);
}
void uwb_node::updatez(double z)
{
   _handle.data_list.Replace("z", z);
}
int uwb_node::setDoubleValue(std::string type, double value)
{
   return 0;
}