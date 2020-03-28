/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-28 11:57:26
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "sensor_node.h"

APP_name::APP_name(ProtocolAnalysis *msg) : Driver_node(msg)
{
}
APP_name::~APP_name()
{
}
void APP_name::initdata()
{
   server_ip = "192.168.2.16";
   server_port = StateMachine_port;
   source_id = ID_Sensor_uwb;

   _handle.driver_name = "LaserLocation";
   _handle.driver_id = 1;
   //添加维护数据
   _handle.data_list.Add("x", _data.x);
   _handle.data_list.Add("y", _data.y);
   _handle.data_list.Add("yaw", _data.z);
}
void APP_name::updateX(double x)
{
   _handle.data_list.Replace("x", x);
}
void APP_name::updatey(double y)
{
   _handle.data_list.Replace("y", y);
}
void APP_name::updateyaw(double z)
{
   _handle.data_list.Replace("yaw", z);
}
int APP_name::setDoubleValue(uint16_t type, double value)
{
   return 0;
}