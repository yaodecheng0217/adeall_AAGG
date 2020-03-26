/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-26 23:03:34
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
   _handle.driver_name = "uwb_sensor";
   _handle.driver_id = 1;
   _handle.driver_id = DIRVER_TYPE::LOCATION;

   server_ip = "127.0.0.1";
   server_port = StateMachine_port;
   source_id = ID_Sensor_uwb;
}
void uwb_node::sendData(uint32_t seq, time_t timestamp)
{

   neb::CJsonObject data;
   data.AddEmptySubObject(_handle.driver_name);
   data[_handle.driver_name].Add("x", 1);
   data[_handle.driver_name].Add("y", 2);
   data[_handle.driver_name].Add("z", 3);
   data[_handle.driver_name].Add("yaw", 3);

   neb::CJsonObject oJson;
   oJson.Add("seq", seq);
   oJson.Add("update",data);

   _msg->sendStringData(server_ip.c_str(),
                        server_port,
                        source_id,
                        INS_LIST::INS_HARBEAT,
                        CMD_TYPE_LIST::CMD_UPDATE_DATA, //设置
                        oJson.ToString());
}
void uwb_node::sendHandle(uint32_t seq)
{
   neb::CJsonObject oJson;
   TYPE_handle_string s;
   oJson.Add("driver_name", _handle.driver_name);
   oJson.Add("driver_id", _handle.driver_id);
   oJson.Add("data_type", _handle.data_type);
   oJson.Add("data_size", _handle.data_size);
   oJson.Add("data_name", _handle.data_name);
   oJson.Add("seq", seq);

   _msg->sendStringData(server_ip.c_str(),
                        server_port,
                        source_id,
                        INS_LIST::INS_HARBEAT,
                        CMD_TYPE_LIST::CMD_HEARBEAT_HANDLE, //设置
                        oJson.ToString());
}
int uwb_node::setDoubleValue(uint16_t type, double value)
{
   return 0;
}