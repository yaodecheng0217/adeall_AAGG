/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-04-09 15:49:28
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "ETV_driver.h"
#include "udpinterface/thread_base.h"
#include "Cjson/CJsonObject.hpp"
#include "readconfig/readjson.h"

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
            if (p->Control_count > 5)
                p->Control_count = 5;
        }
        p->Control_count++;
        Sleep(50);
    }
}
void ETV_driver::initdata()
{
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
    thread_base t3(timer50msthread, this);
    thread_base t2(pidControlthread, this);
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
        _data.lift = value / 5.0;
        return OK;
    }
    else if (type == "side")
    {
        _data.side = value / 5.0;
        return OK;
    }
    else if (type == "forwarld")
    {
        _data.forwarld = value / 5.0;
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
void *ETV_driver::pidControlthread(void *appprt)
{
    double out = 0.01;
    double outV = 0;
    ETV_driver *app = (ETV_driver *)appprt;
    ReadSeting set("seting.json");
    bool print=false;
    set.GetValue("simx_print",print);
    set.setprint(print);
    while (1)
    {
        set.reload();  
        set.GetValue("simx_Vk",app->k);
        if (app->_data.auto_mode)
        {
            if (app->_data.acc == 0)
            {
                outV = 0;
                //pid_info.LocSum = 0;
            }
            out = app->_data.turn;
        }
        else
        {
            //app->_data.turn = app->turnfeedback;
            out = app->_data.turn;
            outV = 0;
            app->_data.acc = 0;
        }

        app->vr->Set_Turn_motor(out);

        app->vr->Set_Acc_motor(app->_data.acc * app->k);

        app->vr->Set_Forward_motor(app->_data.forwarld);

        app->vr->Set_Lift_motor(app->_data.lift);

        app->vr->Set_Side_motor(app->_data.side);
        //printf("%f  %f %f\n", app->_data.acc, app->_data.speed, outV);
        app->timer50L.lock();
    }
}
void *ETV_driver::timer50msthread(void *appprt)
{
    ETV_driver *app = (ETV_driver *)appprt;
    while (1)
    {
        Sleep(50);
        app->timer50L.unlock();
    }
}
void ETV_driver::updateSpeed(double speed)
{
    _data.speed = speed * k;
}