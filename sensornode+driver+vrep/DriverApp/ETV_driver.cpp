/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-21 15:17:14
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "ETV_driver.h"

ETV_driver::ETV_driver(ProtocolAnalysis *msg,int n,vrep_interface *vrep) : Driver_node(msg),vr(vrep)
{
    _handle.driver_id = n;
}

ETV_driver::~ETV_driver()
{
}
void ETV_driver::initdata()
{
    _handle.driver_name = "Driver"; 
    _handle.datatype = DATA_LIST::ETV_DriverState;

    server_ip = "127.0.0.1";
    server_port = StateMachine_port;
    source_id = ID_Sensor_uwb;
    _data.AUTO=1;
}
void ETV_driver::sendData(uint32_t seq, time_t timestamp)
{
    _Send::N_TYPE_ETV_DRIVER_HEARBEAT_DATA hearbeat;
    hearbeat.id = _handle.driver_id;
    hearbeat.data = _data;
    hearbeat.seq = seq;
    hearbeat.timestamp = timestamp;
    hearbeat.state_ok = OK;

    _msg->sendData_N(server_ip.c_str(),
                     server_port,
                     source_id,
                     INS_LIST::INS_HARBEAT,
                     CMD_TYPE_LIST::N_CMD_HEARBEAT_ETV_DRIVER_DATA, //设置
                     hearbeat);
}
void ETV_driver::sendHandle(uint32_t seq)
{
    _Send::TYPE_ETV_DRIVER_HEARBEAT_DATA hearbeat_handle;
    hearbeat_handle.handle = _handle;
    hearbeat_handle.seq = seq;
    _msg->sendData(server_ip.c_str(),
                   server_port,
                   source_id,
                   INS_LIST::INS_HARBEAT,
                   CMD_TYPE_LIST::CMD_HEARBEAT_ETV_DRIVER_DATA, //设置
                   hearbeat_handle);
}
int ETV_driver::setDoubleValue(uint16_t type, double value)
{
    printf("set-->\n");
    switch (type)
    {
    case _data::DATA_TYPE_LIST::Type_AcceleratorValue:
        _data.AcceleratorValue = value;
        vr->Set_Acc_motor(-value);
        break;
    case _data::DATA_TYPE_LIST::Type_BrakeValue:
        _data.BrakeValue = value;
        break;
    case _data::DATA_TYPE_LIST::Type_LiftValue:
        _data.LiftValue = value;
        vr->Set_Lift_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_MoveForwardValue:
        _data.MoveForwardValue = value;
        vr->Set_Forward_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_SideValue:
        _data.SideValue = value;
        vr->Set_Side_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_TiltValue:
        _data.TiltValue = value;
        break;
    case _data::DATA_TYPE_LIST::Type_TurnAngleValue:
        _data.TurnAngleValue = value;
        vr->Set_Turn_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_AUTO:
        _data.AUTO = value;
        break;
    case _data::DATA_TYPE_LIST::Type_LED_Green:
        _data.LED_Green = value;
        break;
    case _data::DATA_TYPE_LIST::Type_LED_Red:
        _data.LED_Red = value;
        break;
    case _data::DATA_TYPE_LIST::Type_Paking:
        _data.Paking = value;
        break;
    default:
        return ERR;
        break;
    }
    return OK;
}