/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-22 15:49:28
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
    _handle.driver_name="uwb_sensor";
    _handle.driver_id= 1;
    _handle.driver_type= DIRVER_TYPE::LOCATION;

    server_ip="192.168.2.16";
    server_port=StateMachine_port;
    source_id=ID_Sensor_uwb;
}
void uwb_node::sendData(uint32_t seq, time_t timestamp)
{
   TYPE_UWB_UPDATE_DATA hearbeat;
    hearbeat.id = _handle.driver_id;
    hearbeat.data = _data;
    hearbeat.seq = seq;
    hearbeat.timestamp = timestamp;
    hearbeat.state_ok = OK;

    _msg->sendData(server_ip.c_str(),
                     server_port,
                     source_id,
                     INS_LIST::INS_HARBEAT,
                     CMD_TYPE_LIST::CMD_HEARBEAT_UWB_DATA,//设置
                     hearbeat);
}
void uwb_node::sendHandle(uint32_t seq)
{
   neb::CJsonObject oJson;
   TYPE_handle_string s;
   oJson.Add(s.driver_name,_handle.driver_name);
   oJson.Add(s.driver_id,_handle.driver_id);
   oJson.Add(s.driver_type,_handle.driver_type);
   oJson.Add(s.seq,seq);

   _msg->sendStringData(server_ip.c_str(),
                   server_port,
                   source_id,
                   INS_LIST::INS_HARBEAT,
                   CMD_TYPE_LIST::CMD_HEARBEAT_HANDLE,//设置
                   oJson.ToString());
}
int uwb_node::setDoubleValue(uint16_t type, double value)
{
   return 0;
}