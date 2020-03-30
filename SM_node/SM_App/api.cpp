/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 10:05:22
 * @LastEditors: Yaodecheng
 */
#include "app.h"

void APP::ACK_One_data(const char *ip, int port, uint16_t type, uint32_t seq)
{
    double value = 0;

    int code = -1;

    time_t timestamp;

    if (type == Type_location)

    {
        Node_INFO *driver = GetNode_INFO("location");
        TYPE_ACK_LOCATION_DATA aa;

        if (driver == NULL)

        {

            //printf("driver is not find!\n");

            code = DriverIsNull;
        }

        else

        {

            LOCATION_DATA info;

            if (driver->handle.data_list.Get("x", info.x) && driver->handle.data_list.Get("y", info.y) && driver->handle.data_list.Get("yaw", info.yaw))
            {
                timestamp = driver->timestamp;
                aa.data = info;
                code = OK;
            }
            else
            {
                code = ERR;
            }
        }

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

        switch (type)
        {
        case Type_AcceleratorValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("acc", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_AUTO:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("auto_mode", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_BrakeValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("break", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_LED_Green:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("ledgreen", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_LED_Red:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("ledred", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_LiftValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("lift", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_MoveForwardValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("forward", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_Paking:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("paking", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_SideValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("side", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_TiltValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("tilt", value))
            {
                code = OK;
                timestamp = driver->timestamp;
            }
        }
        break;
        case Type_TurnAngleValue:
        {
            Node_INFO *driver = GetNode_INFO("Car_control");
            if (driver == NULL)
            {
                printf("driver is not find!\n");
                code = DriverIsNull;
                break;
            }
            if (driver->handle.data_list.Get("turn", value))
            {
                code = OK;
                //printf("ok %f\n", value);
                timestamp = driver->timestamp;
            }
        }
        break;

        case Type_forward_lasser:
        {
             Node_INFO *laser = GetNode_INFO("laser_sensor");
            if (laser == NULL)
            {
                printf("laser is not find!\n");
                code = DriverIsNull;
                break;
            }
             if (laser->handle.data_list.Get("forward_laser", value))
            {
                code = OK;
                //printf("ok %f\n", value);
                timestamp = laser->timestamp;
            }
        }
        break;
       case Type_high_lasser:
        {
             Node_INFO *laser = GetNode_INFO("laser_sensor");
            if (laser == NULL)
            {
                printf("laser is not find!\n");
                code = DriverIsNull;
                break;
            }
             if (laser->handle.data_list.Get("high_laser", value))
            {
                code = OK;
               // printf("ok %f\n", value);
                timestamp = laser->timestamp;
            }
        }
        break;
        case Type_side_lasser:
        {
             Node_INFO *laser = GetNode_INFO("laser_sensor");
            if (laser == NULL)
            {
                printf("laser is not find!\n");
                code = DriverIsNull;
                break;
            }
             if (laser->handle.data_list.Get("side_laser", value))
            {
                code = OK;
                //printf("ok %f\n", value);
                timestamp = laser->timestamp;
            }
        }
        break;
        case Type_TrayL_lasser:
        {
             Node_INFO *laser = GetNode_INFO("laser_sensor");
            if (laser == NULL)
            {
                printf("laser is not find!\n");
                code = DriverIsNull;
                break;
            }
             if (laser->handle.data_list.Get("TrayL_laser", value))
            {
                code = OK;
                //printf("ok %f\n", value);
                timestamp = laser->timestamp;
            }
        }
        break;
        case Type_TrayH_lasser:
        {
             Node_INFO *laser = GetNode_INFO("laser_sensor");
            if (laser == NULL)
            {
                printf("laser is not find!\n");
                code = DriverIsNull;
                break;
            }
             if (laser->handle.data_list.Get("TrayH_laser", value))
            {
                code = OK;
                //rintf("ok %f\n", value);
                timestamp = laser->timestamp;
            }
        }
        break;
        
        default:
            code = ERR;
            break;
        }

        //返回数据

        TYPE_ACK_ONE_DATA aa;
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
    //printf("====================================%d\n", type);
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