/*
 * @Author: Yaodecheng
 * @Date: 2020-03-12 11:19:05
 * @LastEditors: Yaodecheng
 */
//发送数据格式定义

/*
《===========================消息结构说明======================V1.0====================================================

uint8_t source_id = 0;                            //代表信息来源，可以用作参考，但不作为数据结构解析依据
uint8_t cmd_id[2] = {0, 0};                       //空
int8_t ins = -1;                                  //指令信息类型，决定了是什么样的数据
uint8_t cmd_type = 0;                             //数据结构类型，解析时严格按照该类型结构进行数据解析，一种数据结构一种类型
std::vector<uint8_t> _databuff;
====================================================================================================================

==========================数据结构定义举例============================================================================
struct SOMEDATA
{
    //随便定义一些数据，可以使指针，但不可以是容器类
    int a;
    double b;
    std::string c;

   //添加序列化模板函数
   //由于采用cereal库进行序列化不支持原始指针和引用对象的序列化，但智能指针是支持的。
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(a, b, c); //serialize things by passing them to the archive
    };
}
======================================================================================================================
=======================文档更新规范==================
1. 使用命名空间进行相关数据类型分隔
2. 
====================================================
*/

#ifndef _DEFINE_MSG_DATA_
#define _DEFINE_MSG_DATA_

#include <stdint.h>
#include <string>
#include <sys/time.h>

namespace adeall
{
enum PORT_LIST
{
    Debug_port = 9000,
    StateMachine_port = 9001,
    Sensor_port = 9002,
    Driver_port = 9003,
};
//消息来源列表
enum SOURCE_ID_LIST
{
    ID_DEBUG,
    ID_Sensor_uwb,
    ID_StateMachine,
    ID_Controller,
    ID_Simulation,
    ID_PathPlaner,

};
//消息类型
enum INS_LIST
{
    INS_GET,
    INS_SET,
    INS_ACK,
    INS_HARBEAT,
};
//消息结构体列表
enum CMD_TYPE_LIST
{
    CMD_HEARBEAT_ETV_DRIVER_DATA,
    CMD_HEARBEAT_UWB_DATA,
    CMD_HEARBEAT_ONE_DATA,
    CMD_HEARBEAT_SICK_DATA,
    CMD_HEARBEAT_HANDLE,
    CMD_SET_DOUBLE_DATA,
    CMD_GET_DATA,
    CMD_ACK_SET,
    CMD_ACK_LOCATION_DATA,
    CMD_ACK_ONE_DATA,
    CMD_ACK_CODE,
};
//请求返回码
enum STATE_CODE_LIST
{
    OK,
    ERR,
    TIMEOUT,
    InManualState,
    DriverIsNull,
    ForkErr,
};

//========================================消息子结构==========================================
//数据结构体枚举
enum DIRVER_TYPE
{
    LOCATION,
    ETV_Driver,
};
struct LOCATION_DATA
{
    double x, y, z, yaw;
};
struct ETV_DRIVER_STATE_DATA
{
    double AcceleratorValue = 0,
           BrakeValue = 0,
           TurnAngleValue = 0,
           LiftValue = 0,
           SideValue = 0,
           MoveForwardValue = 0,
           TiltValue = 0;
    bool LED_Green = 0,
         LED_Red = 0,
         Paking = 0,
         AUTO = 0;
};
//===============================================================================================
enum DATA_SET_GET_TYPE_LIST
{
    Type_AcceleratorValue,
    Type_BrakeValue,
    Type_TurnAngleValue,
    Type_LiftValue,
    Type_SideValue,
    Type_MoveForwardValue,
    Type_TiltValue,
    Type_LED_Green,
    Type_LED_Red,
    Type_Paking,
    Type_AUTO,

    //====
    Type_location
};
struct DRIVER_HANDLE
{
     std::string driver_name;
     uint32_t driver_id;
     uint32_t driver_type;
};
struct TYPE_handle_string
{
     const std::string driver_name="driver";
     const std::string driver_id="driver_id";
     const std::string driver_type="driver_type";
     const std::string seq="seq";
};
struct TYPE_UWB_UPDATE_DATA
{
    uint32_t id;
    LOCATION_DATA data;
    uint32_t seq;
    bool state_ok;
    time_t timestamp;
};
struct TYPE_ETV_DRIVER_UPDATE_DATA
{
    uint32_t id;
    ETV_DRIVER_STATE_DATA data;
    uint32_t seq;
    bool state_ok;
    time_t timestamp;
};
struct TYPE_SET_DOUBLE_DATA
{
    uint8_t type;
    double value;
    uint32_t seq;
    time_t timestamp;
};
struct TYPE_GET_DATA
{
    uint16_t type;
    uint32_t seq;
    time_t timestamp;
};
struct TYPE_ACK_CODE
{
    uint32_t seq;
    int code;
};
struct TYPE_ACK_ONE_DATA
{
    double data;
    int code;
    uint32_t seq;
    time_t timestamp;
};
struct TYPE_ACK_LOCATION_DATA
{
    LOCATION_DATA data;
    int code;
    uint32_t seq;
    time_t timestamp;
};
} // namespace adeall

#endif
