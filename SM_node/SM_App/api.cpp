/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 10:05:22
 * @LastEditors: Yaodecheng
 */
#include "app.h"
void APP::Get_Information(const char *ip, int port, uint16_t type, uint32_t seq)
{
    int code = -1;
    double value = 0;
    switch (type)
    {
    case DATA_SET_GET_TYPE_LIST::Type_AcceleratorValue:
    case DATA_SET_GET_TYPE_LIST::Type_AUTO:
    case DATA_SET_GET_TYPE_LIST::Type_BrakeValue:
    case DATA_SET_GET_TYPE_LIST::Type_LED_Green:
    case DATA_SET_GET_TYPE_LIST::Type_LED_Red:
    case DATA_SET_GET_TYPE_LIST::Type_LiftValue:
    case DATA_SET_GET_TYPE_LIST::Type_MoveForwardValue:
    case DATA_SET_GET_TYPE_LIST::Type_Paking:
    case DATA_SET_GET_TYPE_LIST::Type_SideValue:
    case DATA_SET_GET_TYPE_LIST::Type_TiltValue:
    case DATA_SET_GET_TYPE_LIST::Type_TurnAngleValue:
        break;
    default:
        break;
    }
}

void APP::ACK_One_data(const char *ip, int port, uint16_t type, uint32_t seq)
{
    
}
int APP::set_ControlValue(uint16_t type, double value)
{
    //printf("====================================%d\n",type);
    switch (type)
    {
    case DATA_SET_GET_TYPE_LIST::Type_AcceleratorValue:
        return SetAcceleratorValue(value);
    case DATA_SET_GET_TYPE_LIST::Type_AUTO:
        return SetAuto(value);
    case DATA_SET_GET_TYPE_LIST::Type_BrakeValue:
        return SetBrake(value);
    case DATA_SET_GET_TYPE_LIST::Type_LED_Green:
        return SetLedGreen(value);
    case DATA_SET_GET_TYPE_LIST::Type_LED_Red:
        return SetLedRed(value);
    case DATA_SET_GET_TYPE_LIST::Type_LiftValue:
        return SetLift(value);
    case DATA_SET_GET_TYPE_LIST::Type_MoveForwardValue:
        return SetMoveForward(value);
    case DATA_SET_GET_TYPE_LIST::Type_Paking:
        return SetPacking(value);
    case DATA_SET_GET_TYPE_LIST::Type_SideValue:
        return SetSide(value);
    case DATA_SET_GET_TYPE_LIST::Type_TiltValue:
        return SetTilt(value);
    case DATA_SET_GET_TYPE_LIST::Type_TurnAngleValue:
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
    TYPE_ACK_CODE aa;
    aa.code = code;
    aa.seq = seq;
    //printf("%d   \n", seq,code);
    _msg->sendData(ip,
                   port,
                   ID_StateMachine,
                   INS_ACK,
                   CMD_ACK_SET,
                   aa);
}