/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-30 15:32:49
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
        if (p->Control_count > 3)
        {
            p->_data.acc = 0;
            p->_data.lift = 0;
            p->_data.side = 0;
            p->_data.forwarld = 0;
            p->Control_count--;
        }
        p->Control_count++;
        p->vr->Set_Acc_motor(p->_data.acc);
        p->vr->Set_Forward_motor(p->_data.forwarld);
        p->vr->Set_Lift_motor(p->_data.lift);
        p->vr->Set_Side_motor(p->_data.side);
        p->vr->Set_Turn_motor(p->_data.turn+5/57.3);
        Sleep(50);
    }
}
void ETV_driver::initdata()
{
    server_ip = "127.0.0.1";
    server_port = StateMachine_port;
    source_id = ID_Sensor_uwb;

    _handle.driver_name = "Car_control";
    _handle.driver_id = 1;

    //添加维护数据
    _handle.data_list.Add("acc", _data.acc);
    _handle.data_list.Add("turn", _data.turn);
    _handle.data_list.Add("lift", _data.lift);
    _handle.data_list.Add("forwarld", _data.forwarld);
    _handle.data_list.Add("side", _data.side);
    _handle.data_list.Add("auto_mode", _data.auto_mode);
    _data.auto_mode = 1;
    thread_base t(controlOnline, this);
}
void ETV_driver::datalist_up()
{
    _handle.data_list.Replace("acc", _data.acc);
    _handle.data_list.Replace("turn", _data.turn);
    _handle.data_list.Replace("lift", _data.lift);
    _handle.data_list.Replace("forwarld", _data.forwarld);
    _handle.data_list.Replace("side", _data.side);
    _handle.data_list.Replace("auto_mode", _data.auto_mode);
}
int ETV_driver::setDoubleValue(std::string type, double value)
{
    Control_count = 0;
    if (type == "acc")
    {
        _data.acc = -value;
        return OK;
    }
    else if (type == "lift")
    {
        _data.lift = value/5.0;
        return OK;
    }
    else if (type == "side")
    {
        _data.side = value/5.0;
        return OK;
    }
    else if (type == "forwarld")
    {
        _data.forwarld = value/5.0;
        return OK;
    }
    else if (type == "turn")
    {
        _data.turn = value;
        return OK;
    }
    else if (type == "turn")
    {
        _data.turn = value;
        return OK;
    }
    else if (type == "auto_mode")
    {
        _data.auto_mode = value;
        return OK;
    }
    return ERR;
}