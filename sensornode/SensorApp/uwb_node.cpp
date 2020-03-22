/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-22 15:49:28
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "uwb_node.h"
#include "CJson/cJSON.h"
uwb_node::uwb_node(ProtocolAnalysis *msg) : Sensor_node(msg)
{
}
uwb_node::~uwb_node()
{
}
void uwb_node::initdata()
{
    _handle.driver_name="uwb_sensor";
    _handle.driver_id= 1;
    _handle.datatype= DATA_LIST::LOCATION;

    server_ip="192.168.147.25";
    server_port=StateMachine_port;
    source_id=ID_Sensor_uwb;
}
void uwb_node::updateData(uint32_t seq, time_t timestamp)
{
    
}
void uwb_node::sendData(uint32_t seq, time_t timestamp)
{
    _Send::N_TYPE_UWB_HEARBEAT_DATA hearbeat;
    hearbeat.id = _handle.driver_id;
    hearbeat.data = _data;
    hearbeat.seq = seq;
    hearbeat.timestamp = timestamp;
    hearbeat.state_ok = OK;

    _msg->sendData_N(server_ip.c_str(),
                     server_port,
                     source_id,
                     INS_LIST::INS_HARBEAT,
                     CMD_TYPE_LIST::N_CMD_HEARBEAT_UWB_DATA,//设置
                     hearbeat);
}
void uwb_node::sendHandle(uint32_t seq)
{
   neb::CJsonObject oJson;
   oJson.Add("name",_handle.driver_name);
   oJson.Add("id",_handle.driver_id);
   oJson.Add("type",_handle.datatype);
   oJson.Add("seq",seq);

   _msg->sendStringData(server_ip.c_str(),
                   server_port,
                   source_id,
                   INS_LIST::INS_HARBEAT,
                   CMD_TYPE_LIST::CMD_HEARBEAT_UWB_DATA,//设置
                   oJson.ToString());

   printf("\n%s\n",oJson.ToString().c_str());
}