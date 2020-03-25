/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-25 15:39:05
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "lasser_node.h"

lasser_node::lasser_node(ProtocolAnalysis *msg,std::string name,uint8_t id) : Driver_node(msg)
{
   _handle.driver_id=id;
   _handle.driver_name=name;
}
lasser_node::~lasser_node()
{
}
void lasser_node::initdata()
{
    
    //_handle.driver_id= 1;
    _handle.driver_type= DIRVER_TYPE::DOUBLE_DATA;

    server_ip="192.168.2.16";
    server_port=StateMachine_port;
    source_id=ID_Sensor_uwb;
}
void lasser_node::sendData(uint32_t seq, time_t timestamp)
{
   TYPE_DOUBLE_UPDATE_DATA hearbeat;
    hearbeat.id = _handle.driver_id;
    hearbeat.data = _data;
    hearbeat.seq = seq;
    hearbeat.timestamp = timestamp;
    hearbeat.state_ok = OK;

    _msg->sendData(server_ip.c_str(),
                     server_port,
                     source_id,
                     INS_LIST::INS_HARBEAT,
                     CMD_TYPE_LIST::CMD_HEARBEAT_DOUBLE_DATA,//设置
                     hearbeat);
}
void lasser_node::sendHandle(uint32_t seq)
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
int lasser_node::setDoubleValue(uint16_t type, double value)
{
   return 0;
}