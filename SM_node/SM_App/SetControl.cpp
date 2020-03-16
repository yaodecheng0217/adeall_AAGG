/*
 * @Author: Yaodecheng
 * @Date: 2020-03-15 17:54:06
 * @LastEditTime: 2020-03-16 18:56:42
 * @LastEditors: Yaodecheng
 */
#include "app.h"

using namespace _data;
void *APP::ETV_DriverOnlineChack()
{
    size_t cnt = _NodeList.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.datatype == DATA_LIST::ETV_DriverState)
        {
            return &_NodeList[i];
        }
    }
    return NULL;
}
int APP::sendToDriver(uint8_t type, double value)
{
    timeval tv;
    gettimeofday(&tv, NULL);
    _Send::TYPE_SET_DOUBLE_DATA setdata;
    setdata.handle = StateMachine;
    SET_DOUBLE_DATA data;
    data.type=type;
    data.value=value;
    setdata.data = data;
    setdata.seq = _seq++;
    setdata.timestamp = (time_t)tv.tv_sec * (time_t)1000000 + (time_t)tv.tv_usec;

    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = setdata.seq;
    _respondlist.push_back(w);
    //printf("%d\n", w.seq);

    for (size_t j = 0; j < 3; j++)
    {
        _msg->sendData("127.0.0.1",
                       PORT_LIST::Driver_port,
                       SOURCE_ID_LIST::ID_StateMachine,
                       INS_LIST::INS_SET,
                       CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA,
                       setdata);
        for (size_t i = 0; i < 50; i++)
        {
            if (code != -1)
            {
                clear_sqe(w.seq);
                return code;
            }
            Sleep(1);
        }
        //printf("%d retry %d times,\n", w.seq, j + 1);
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
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        return sendToDriver(Type_AcceleratorValue, value);
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
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        return sendToDriver(Type_BrakeValue, value);
    }
}

int APP::SetTurnAngle(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        return sendToDriver(Type_TurnAngleValue, value);
    }
}

int APP::SetLift(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        if (info.MoveForwardValue != 0 || info.TiltValue != 0 || info.SideValue != 0)
        {
            return ForkErr;
        }
        return sendToDriver(Type_LiftValue, value);
    }
}
int APP::SetSide(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        if (info.MoveForwardValue != 0 || info.TiltValue != 0 || info.LiftValue != 0)
        {
            return ForkErr;
        }
        return sendToDriver(Type_SideValue, value);
    }
}
int APP::SetMoveForward(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        if (info.SideValue != 0 || info.TiltValue != 0 || info.LiftValue != 0)
        {
            return ForkErr;
        }
        return sendToDriver(Type_MoveForwardValue, value);
    }
}
int APP::SetTilt(double value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        if (info.AUTO == 0)
        {
            return InManualState;
        }
        if (info.MoveForwardValue != 0 || info.TiltValue != 0 || info.LiftValue != 0)
        {
            return ForkErr;
        }
        return sendToDriver(Type_TiltValue, value);
    }
}
int APP::SetLedGreen(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        return sendToDriver(Type_LED_Green, value);
    }
}
int APP::SetLedRed(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        return sendToDriver(Type_LED_Red, value);
    }
}
int APP::SetPacking(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        return sendToDriver(Type_Paking, value);
    }
}
int APP::SetAuto(bool value)
{
    void *driver = ETV_DriverOnlineChack();
    if (driver == NULL)
    {
        printf("etv_driver is not find!\n");
        return DriverIsNull;
    }
    else
    {
        _data::ETV_DRIVER_STATE_DATA info = *(_data::ETV_DRIVER_STATE_DATA *)(*(Node_INFO *)driver).data;
        return sendToDriver(Type_AUTO, value);
    }
}