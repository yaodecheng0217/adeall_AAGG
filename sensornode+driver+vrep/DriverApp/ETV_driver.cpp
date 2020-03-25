/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-25 15:25:14
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "ETV_driver.h"
#include "udpinterface/thread_base.h"
ETV_driver::ETV_driver(ProtocolAnalysis *msg, int n, vrep_interface *vrep) : Driver_node(msg), vr(vrep)
{
    _handle.driver_id = n;
}

ETV_driver::~ETV_driver()
{
}
void *ETV_driver::controlOnline(void *etv)
{
    ETV_driver *p = (ETV_driver *)etv;
    while (true)
    {
        if (p->Control_count > 5)
        {
            p->vr->Set_Acc_motor(0);
            p->vr->Set_Forward_motor(0);
            p->vr->Set_Lift_motor(0);
            p->vr->Set_Side_motor(0);
            p->Control_count--;
        }
        p->Control_count++;
        Sleep(100);
    }
}
void ETV_driver::initdata()
{
    _handle.driver_name = "Driver";
    _handle.driver_type = DIRVER_TYPE::ETV_Driver;

    server_ip = "192.168.2.16";
    server_port = StateMachine_port;
    source_id = ID_Sensor_uwb;
    _data.AUTO = 1;
    thread_base t(controlOnline, this);
}
void ETV_driver::sendData(uint32_t seq, time_t timestamp)
{
    TYPE_ETV_DRIVER_UPDATE_DATA hearbeat;
    hearbeat.id = _handle.driver_id;
    hearbeat.data = _data;
    hearbeat.seq = seq;
    hearbeat.timestamp = timestamp;
    hearbeat.state_ok = OK;

    _msg->sendData(server_ip.c_str(),
                   server_port,
                   source_id,
                   INS_LIST::INS_HARBEAT,
                   CMD_TYPE_LIST::CMD_HEARBEAT_ETV_DRIVER_DATA, //设置
                   hearbeat);
}
void ETV_driver::sendHandle(uint32_t seq)
{
    neb::CJsonObject oJson;
    TYPE_handle_string s;
    oJson.Add(s.driver_name, _handle.driver_name);
    oJson.Add(s.driver_id, _handle.driver_id);
    oJson.Add(s.driver_type, _handle.driver_type);
    oJson.Add(s.seq, seq);

    _msg->sendStringData(server_ip.c_str(),
                         server_port,
                         source_id,
                         INS_LIST::INS_HARBEAT,
                         CMD_TYPE_LIST::CMD_HEARBEAT_HANDLE, //设置
                         oJson.ToString());

    // printf("\n%s\n",oJson.ToString().c_str());
}
int ETV_driver::setDoubleValue(uint16_t type, double value)
{
    Control_count = 0;
    switch (type)
    {
    case DATA_SET_GET_TYPE_LIST::Type_AcceleratorValue:
        if (value != _data.AcceleratorValue);
        vr->Set_Acc_motor(-value);
        _data.AcceleratorValue = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_BrakeValue:
        _data.BrakeValue = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_LiftValue:
        _data.LiftValue = value;
        vr->Set_Lift_motor(value);
        break;
    case DATA_SET_GET_TYPE_LIST::Type_MoveForwardValue:
        _data.MoveForwardValue = value;
        vr->Set_Forward_motor(value);
        break;
    case DATA_SET_GET_TYPE_LIST::Type_SideValue:
        _data.SideValue = value;
        vr->Set_Side_motor(value);
        break;
    case DATA_SET_GET_TYPE_LIST::Type_TiltValue:
        _data.TiltValue = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_TurnAngleValue:
        if (value != _data.TurnAngleValue)
            vr->Set_Turn_motor(value);
        _data.TurnAngleValue = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_AUTO:
        _data.AUTO = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_LED_Green:
        _data.LED_Green = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_LED_Red:
        _data.LED_Red = value;
        break;
    case DATA_SET_GET_TYPE_LIST::Type_Paking:
        _data.Paking = value;
        break;
    default:
        return ERR;
        break;
    }
    return OK;
}