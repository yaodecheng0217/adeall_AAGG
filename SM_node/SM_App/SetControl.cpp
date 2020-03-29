/*
 * @Author: Yaodecheng
 * @Date: 2020-03-15 17:54:06
 * @LastEditTime: 2020-03-30 00:02:26
 * @LastEditors: Yaodecheng
 */
#include "app.h"

#define Contrl_driver_name "Car_control"

int APP::sendToDriver(const char *ip, int port, uint8_t type, double value)
{
    timeval tv;
    gettimeofday(&tv, NULL);
    TYPE_SET_DOUBLE_DATA setdata;
    setdata.type = type;
    setdata.value = value;
    setdata.seq = _seq++;
    setdata.timestamp = (time_t)tv.tv_sec * (time_t)1000000 + (time_t)tv.tv_usec;

    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = setdata.seq;
    _rslist_lock.lock();
    _respondlist.push_back(w);
    _rslist_lock.unlock();

    _msg->sendData(ip,
                   port,
                   SOURCE_ID_LIST::ID_StateMachine,
                   INS_LIST::INS_SET,
                   CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA,
                   setdata);

    if (waitForACK(w.seq, w.code, 20))
    {
        clear_sqe(w.seq);
        return *w.code;
    }
    printf("time out ------------------------%d\n", w.seq);
    clear_sqe(w.seq);
    return TIMEOUT;
}
int APP::sendToDriver(const char *ip, int port, std::string type, double value)
{
    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = _seq++;
    _rslist_lock.lock();
    _respondlist.push_back(w);
    _rslist_lock.unlock();

    neb::CJsonObject set;
    set.Add("seq", w.seq);
    set.Add("type", type);
    set.Add("value", value);

    _msg->sendStringData(ip,
                         port,
                         SOURCE_ID_LIST::ID_StateMachine,
                         INS_LIST::INS_SET,
                         CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA,
                         set.ToString());

    if (waitForACK(w.seq, w.code, 20))
    {
        clear_sqe(w.seq);
        return *w.code;
    }
    printf("set ctrl timeout%d\n", w.seq);
    clear_sqe(w.seq);
    return TIMEOUT;
}
int APP::SetAcceleratorValue(double value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get("acc", x))
        {
            return sendToDriver(info->ip.c_str(), info->port, "acc", value);
        }
        return ERR;
    }
}

int APP::SetBrake(double value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "brake";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}

int APP::SetTurnAngle(double value)
{
   Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "turn";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}

int APP::SetLift(double value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "lift";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        printf("list>>>>\n");
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetSide(double value)
{
   Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "side";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetMoveForward(double value)
{
   Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "forward";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetTilt(double value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "tilt";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetLedGreen(bool value)
{
   Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "ledgreen";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetLedRed(bool value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "lesred";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetPacking(bool value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "packing";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}
int APP::SetAuto(bool value)
{
    Node_INFO *info = GetNode_INFO(Contrl_driver_name);
    std::string type = "auto_mode";
    if (info == NULL)
    {
        printf("driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        double x;
        if (info->handle.data_list.Get(type, x))
        {
            return sendToDriver(info->ip.c_str(), info->port, type, value);
        }
        return ERR;
    }
}