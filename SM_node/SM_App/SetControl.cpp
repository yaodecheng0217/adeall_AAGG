/*
 * @Author: Yaodecheng
 * @Date: 2020-03-15 17:54:06
 * @LastEditTime: 2020-03-26 22:37:21
 * @LastEditors: Yaodecheng
 */
#include "app.h"

void *APP::ETV_DriverOnlineChack()
{
    ScopeLocker K(&info_lock);
    size_t cnt = _NodeList.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_id == ETV_Driver)
        {
            return &_NodeList[i];
        }
    }
    return NULL;
}
void *APP::UWB_DriverOnlineChack()
{
    ScopeLocker K(&info_lock);
    size_t cnt = _NodeList.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_id == LOCATION)
        {
            return &_NodeList[i];
        }
    }
    return NULL;
}
 Node_INFO *APP::GetNode_INFO(uint16_t  type,uint32_t driver_id)
 {
    ScopeLocker K(&info_lock);
    size_t cnt = _NodeList.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_id == type&&_NodeList[i].handle.driver_id==driver_id)
        {
            return &_NodeList[i];
        }
    }
    return NULL;
 }
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
int APP::SetAcceleratorValue(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                ETV_DRIVER_STATE_DATA info = _driverdata[i].data;
                if (info.AUTO == 0)
                {
                    return InManualState;
                }
                return sendToDriver((*(Node_INFO *)driver).ip.c_str(), (*(Node_INFO *)driver).port, Type_AcceleratorValue, value);
            }
        }
        return ERR;
    }
}

int APP::SetBrake(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                ETV_DRIVER_STATE_DATA info = _driverdata[i].data;
                if (info.AUTO == 0)
                {
                    return InManualState;
                }
                return sendToDriver((*(Node_INFO *)driver).ip.c_str(), (*(Node_INFO *)driver).port, Type_BrakeValue, value);
            }
        }
        return ERR;
    }
}

int APP::SetTurnAngle(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                if (_driverdata[i].data.AUTO == 0)
                {
                    return InManualState;
                }
                return sendToDriver((*(Node_INFO *)driver).ip.c_str(), (*(Node_INFO *)driver).port, Type_TurnAngleValue, value);
            }
        }
        return ERR;
    }
}

int APP::SetLift(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {

        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                ETV_DRIVER_STATE_DATA info = _driverdata[i].data;
                if (info.AUTO == 0)
                {
                    return InManualState;
                }
                if (info.MoveForwardValue != 0 || info.TiltValue != 0 || info.SideValue != 0)
                {
                    return ForkErr;
                }
                return sendToDriver((*(Node_INFO *)driver).ip.c_str(), (*(Node_INFO *)driver).port, Type_LiftValue, value);
            }
        }
        return ERR;
    }
}
int APP::SetSide(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {

        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                ETV_DRIVER_STATE_DATA info = _driverdata[i].data;
                if (info.AUTO == 0)
                {
                    return InManualState;
                }
                if (info.MoveForwardValue != 0 || info.TiltValue != 0 || info.LiftValue != 0)
                {
                    return ForkErr;
                }
                return sendToDriver((*(Node_INFO *)driver).ip.c_str(), (*(Node_INFO *)driver).port, Type_SideValue, value);
            }
        }
        return ERR;
    }
}
int APP::SetMoveForward(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {

        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                ETV_DRIVER_STATE_DATA info = _driverdata[i].data;
                if (info.AUTO == 0)
                {
                    return InManualState;
                }
                if (info.SideValue != 0 || info.TiltValue != 0 || info.LiftValue != 0)
                {
                    return ForkErr;
                }
                return sendToDriver((*(Node_INFO *)driver).ip.c_str(), (*(Node_INFO *)driver).port, Type_MoveForwardValue, value);
            }
        }
        return ERR;
    }
}
int APP::SetTilt(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {

        Node_INFO *handle = (Node_INFO *)driver;
        for (size_t i = 0; i < _driverdata.size(); i++)
        {
            if (_driverdata[i].id == handle->handle.driver_id)
            {
                ETV_DRIVER_STATE_DATA info = _driverdata[i].data;
                if (info.AUTO == 0)
                {
                    return InManualState;
                }
                if (info.MoveForwardValue != 0 || info.TiltValue != 0 || info.LiftValue != 0)
                {
                    return ForkErr;
                }
                return sendToDriver(handle->ip.c_str(), handle->port, Type_TiltValue, value);
            }
        }
        return ERR;
    }
}
int APP::SetLedGreen(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        return sendToDriver(handle->ip.c_str(), handle->port, Type_LED_Green, value);
    }
}
int APP::SetLedRed(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        return sendToDriver(handle->ip.c_str(), handle->port, Type_LED_Red, value);
    }
}
int APP::SetPacking(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        return sendToDriver(handle->ip.c_str(), handle->port, Type_Paking, value);
    }
}
int APP::SetAuto(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        //printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        Node_INFO *handle = (Node_INFO *)driver;
        return sendToDriver(handle->ip.c_str(), handle->port, Type_AUTO, value);
    }
}