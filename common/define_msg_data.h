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
    ID_DEBUG ,
    ID_Sensor_uwb ,
    ID_StateMachine ,
    ID_Controller ,
    ID_Simulation ,
    ID_PathPlaner ,

};
//消息类型
enum INS_LIST
{
    INS_GET ,
    INS_SET ,
    INS_ACK ,
    INS_HARBEAT ,
};
//消息结构体列表
enum CMD_TYPE_LIST
{
    CMD_UWB_HEARBEAT_DATA ,
    CMD_ALL_ACK_DATA ,
    CMD_HEARBEAT_ACK ,
    CMD_ETV_DRIVER_HEARBEAT_DATA ,
    CMD_SET_DOUBLE_DATA ,
    CMD_SET_BOOL_DATA ,
    CMD_SET_FLOAT_DATA ,
    CMD_SET_ACK,
};
//请求返回码
enum STATE_CODE_LIST
{
    OK,
    ERR ,
    TIMEOUT ,
    InManualState,
    DriverIsNull,
    ForkErr,
};
//数据结构体枚举
enum DATA_LIST
{
    LOCATION ,
    ETV_DriverState ,
};
//========================================消息子结构==========================================
struct DRIVER_HANDLE
{
    std::string driver_name;
    uint32_t driver_id;
    int datatype;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(driver_name, driver_id, datatype); //serialize things by passing them to the archive
    }
};
namespace _data
{
struct LOCATION_DATA
{
    double x, y, z, yaw;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(x, y, z, yaw); //serialize things by passing them to the archive
    }
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
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(AcceleratorValue,
                BrakeValue,
                TurnAngleValue,
                LiftValue,
                SideValue,
                MoveForwardValue,
                TiltValue,
                LED_Green,
                LED_Red,
                Paking,
                AUTO); //serialize things by passing them to the archive
    }
};
enum SET_DATA_TYPE_LIST
{
    Type_AcceleratorValue,
    Type_BrakeValue ,
    Type_TurnAngleValue,
    Type_LiftValue ,
    Type_SideValue ,
    Type_MoveForwardValue ,
    Type_TiltValue ,
    Type_LED_Green,
    Type_LED_Red ,
    Type_Paking ,
    Type_AUTO ,
};
struct SET_DOUBLE_DATA
{
    uint8_t type;
    double value;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(type, value);
    }
};
struct SET_BOOL_DATA
{
    uint8_t type;
    bool value;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(type, value);
    }
};
struct SET_FLOAT_DATA
{
    uint8_t type;
    bool value;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(type, value);
    }
};
} // namespace _data
//============================================================================================

//======================所有可发送消息结构========================================================
namespace _Send
{
struct TYPE_HEARBEAT_ACK
{
    DRIVER_HANDLE handle;
    uint32_t seq;
    int code;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(handle, seq, code); //serialize things by passing them to the archive
    }
};
struct TYPE_SET_ACK
{
    DRIVER_HANDLE handle;
    uint32_t seq;
    int code;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(handle, seq, code); //serialize things by passing them to the archive
    }
};
struct TYPE_UWB_HEARBEAT_DATA
{
    DRIVER_HANDLE handle;
    _data::LOCATION_DATA data;
    uint32_t seq;
    bool state_ok;
    time_t timestamp;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(handle, data, seq, state_ok, timestamp); //serialize things by passing them to the archive
    }
};
struct TYPE_ETV_DRIVER_HEARBEAT_DATA
{
    DRIVER_HANDLE handle;
    _data::ETV_DRIVER_STATE_DATA data;
    uint32_t seq;
    bool state_ok;
    time_t timestamp;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(handle, data, seq, state_ok, timestamp); //serialize things by passing them to the archive
    }
};
struct TYPE_SET_DOUBLE_DATA
{
    DRIVER_HANDLE handle;
    _data::SET_DOUBLE_DATA data;
    uint32_t seq;
    time_t timestamp;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(handle, data, seq, timestamp); //serialize things by passing them to the archive
    }
};
struct TYPE_SET_BOOL_DATA
{
    DRIVER_HANDLE handle;
    _data::SET_BOOL_DATA data;
    uint32_t seq;
    time_t timestamp;
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(handle, data, seq, timestamp); //serialize things by passing them to the archive
    }
};
} // namespace _Send
//=====================================================================================================
} // namespace adeall

#endif
