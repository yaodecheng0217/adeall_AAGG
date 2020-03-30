/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-30 10:42:10
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

   _handle.driver_name = "location";
   _handle.driver_id = 1;
   //添加维护数据
   _handle.data_list.Add("x", _data.x);
   _handle.data_list.Add("y", _data.y);
   _handle.data_list.Add("yaw", _data.yaw);
}
void APP_name::datalist_up()
{
   _handle.data_list.Replace("x", _data.x);
   _handle.data_list.Replace("y", _data.y);
   _handle.data_list.Replace("yaw", _data.yaw);
}
void APP_name::updata(double x,double y,double yaw)
{
   _data.x=x;
   _data.y=y;
   _data.yaw=yaw;
}
int APP_name::setDoubleValue(std::string type, double value)
{
   return 0;
}