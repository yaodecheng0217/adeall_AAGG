/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 10:05:22
 * @LastEditors: Yaodecheng
 */
#include "app.h"
using namespace _data;
void APP::Get_Information(const char *ip, int port, uint16_t type, uint32_t seq)
{
    int code = -1;
    double value = 0;
    switch (type)
    {
    case DATA_TYPE_LIST::Type_AcceleratorValue:
    case DATA_TYPE_LIST::Type_AUTO:
    case DATA_TYPE_LIST::Type_BrakeValue:
    case DATA_TYPE_LIST::Type_LED_Green:
    case DATA_TYPE_LIST::Type_LED_Red:
    case DATA_TYPE_LIST::Type_LiftValue:
    case DATA_TYPE_LIST::Type_MoveForwardValue:
    case DATA_TYPE_LIST::Type_Paking:
    case DATA_TYPE_LIST::Type_SideValue:
    case DATA_TYPE_LIST::Type_TiltValue:
    case DATA_TYPE_LIST::Type_TurnAngleValue:
        break;
    default:
        break;
    }
}

void APP::ACK_One_data(const char *ip, int port, uint16_t type, uint32_t seq)
{
    double value = 0;
    int code = -1;
    time_t timestamp;

    if (type == _data::DATA_TYPE_LIST::Type_location)
    {
        void *driver = UWB_DriverOnlineChack();
        _Send::TYPE_ACK_LOCATION_DATA aa;
        if (driver == NULL)
        {
            //printf("driver is not find!\n");
            code = DriverIsNull;
        }
        else
        {
            _data::LOCATION_DATA info;
            if (GetDataDetail(((Node_INFO *)driver)->handle.driver_id, &info))
            {
                timestamp = (*(Node_INFO *)driver).timestamp;
                aa.data = info;
                code = OK;
            }
            else
            {
                code = ERR;
            }
        }
        aa.handle = StateMachine;
        aa.code = code;
        aa.seq = seq;
        aa.timestamp = timestamp;
        //printf("%d\n", seq);
        _msg->sendData(ip,
                       port,
                       ID_StateMachine,
                       INS_ACK,
                       CMD_ACK_LOCATION_DATA,
                       aa);
    }
    else
    {
        void *driver = ETV_DriverOnlineChack();
        if (driver == NULL)
        {
            //printf("driver is not find!\n");
            code = DriverIsNull;
        }
        else
        {

           _data::ETV_DRIVER_STATE_DATA info;
            if (GetDataDetail(((Node_INFO *)driver)->handle.driver_id, &info))
            {
                switch (type)
                    {
                    case DATA_TYPE_LIST::Type_AcceleratorValue:
                        value = info.AcceleratorValue;
                        break;
                    case DATA_TYPE_LIST::Type_AUTO:
                        value = info.AUTO;
                        break;
                    case DATA_TYPE_LIST::Type_BrakeValue:
                        value = info.BrakeValue;
                        break;
                    case DATA_TYPE_LIST::Type_LED_Green:
                        value = info.LED_Green;
                        break;
                    case DATA_TYPE_LIST::Type_LED_Red:
                        value = info.LED_Red;
                        break;
                    case DATA_TYPE_LIST::Type_LiftValue:
                        value = info.LiftValue;
                        break;
                    case DATA_TYPE_LIST::Type_MoveForwardValue:
                        value = info.MoveForwardValue;
                        break;
                    case DATA_TYPE_LIST::Type_Paking:
                        value = info.Paking;
                        break;
                    case DATA_TYPE_LIST::Type_SideValue:
                        value = info.SideValue;
                        break;
                    case DATA_TYPE_LIST::Type_TiltValue:
                        value = info.TiltValue;
                        break;
                    case DATA_TYPE_LIST::Type_TurnAngleValue:
                        value = info.TurnAngleValue;
                        break;
                    default:
                        break;
                    }
                    code = OK;
                    timestamp = (*(Node_INFO *)driver).timestamp;
            }
            else
            {
                code = ERR;
            }       
        }
        //返回数据
        _Send::TYPE_ACK_ONE_DATA aa;
        aa.handle = StateMachine;
        aa.code = code;
        aa.data = value;
        aa.seq = seq;
        aa.timestamp = timestamp;
        //printf("%d\n", seq);
        _msg->sendData(ip,
                       port,
                       ID_StateMachine,
                       INS_ACK,
                       CMD_ACK_ONE_DATA,
                       aa);
    }
}
int APP::set_ControlValue(uint16_t type, double value)
{
    //printf("====================================%d\n",type);
    switch (type)
    {
    case DATA_TYPE_LIST::Type_AcceleratorValue:
        return SetAcceleratorValue(value);
    case DATA_TYPE_LIST::Type_AUTO:
        return SetAuto(value);
    case DATA_TYPE_LIST::Type_BrakeValue:
        return SetBrake(value);
    case DATA_TYPE_LIST::Type_LED_Green:
        return SetLedGreen(value);
    case DATA_TYPE_LIST::Type_LED_Red:
        return SetLedRed(value);
    case DATA_TYPE_LIST::Type_LiftValue:
        return SetLift(value);
    case DATA_TYPE_LIST::Type_MoveForwardValue:
        return SetMoveForward(value);
    case DATA_TYPE_LIST::Type_Paking:
        return SetPacking(value);
    case DATA_TYPE_LIST::Type_SideValue:
        return SetSide(value);
    case DATA_TYPE_LIST::Type_TiltValue:
        return SetTilt(value);
    case DATA_TYPE_LIST::Type_TurnAngleValue:
        return SetTurnAngle(value);
        break;
    default:
        break;
    }
    return ERR;
}
void APP::Set_ACK(const char *ip, int port, int code, uint32_t seq)
{
    //返回数据
    _Send::TYPE_SET_ACK aa;
    aa.handle = StateMachine;
    aa.code = code;
    aa.seq = seq;
    //printf("%d\n", seq);
    _msg->sendData(ip,
                   port,
                   ID_StateMachine,
                   INS_ACK,
                   CMD_ACK_SET,
                   aa);
}