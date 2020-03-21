/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-03-21 13:30:19
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "uwb_node.h"

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

    server_ip="127.0.0.1";
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
    _Send::TYPE_UWB_HEARBEAT_DATA hearbeat_handle;
    hearbeat_handle.handle=_handle;
    hearbeat_handle.seq=seq;
    _msg->sendData(server_ip.c_str(),
                   server_port,
                   source_id,
                   INS_LIST::INS_HARBEAT,
                   CMD_TYPE_LIST::CMD_HEARBEAT_UWB_DATA,//设置
                   hearbeat_handle);
}