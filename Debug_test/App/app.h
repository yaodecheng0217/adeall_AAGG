/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:15
 * @LastEditTime: 2020-03-17 11:49:58
 * @LastEditors: Yaodecheng
 */
/*
如遇到缺少 sys/cdefs.h 的情况，输入以下命令安装标c库
sudo apt-get install  build-essential libc6-dev libc6-dev-i386
如遇到缺少 bits/c++config.h的情况，输入以下命令安装gcc编译相关库
sudo  apt-get  install   gcc-7-multilib g++-7-multilib
*/
#include "ProtocolAnalysis.h"
#include <string>
#include "define_msg_data.h"
#include <vector>
#include "thread_base.h"
#include <list>

#ifndef _APP_H_
#define _APP_H_

using namespace msgpa;
using namespace adeall;
std::string printf_status(int code);
struct Node_INFO
{
    DRIVER_HANDLE handle;
    char *ip;
    int port;
    void *data;
    time_t timestamp;
    int onlinecnt;
};
struct UWB_D
{
    uint8_t id;
    _data::LOCATION_DATA data;
};
struct DRIVER_D
{
    uint8_t id;
    _data::ETV_DRIVER_STATE_DATA data;
};
typedef int (*RecallFun)(void *in, void *out);
struct ReCall
{
    uint32_t seq;
    int *flag;
    void *out;
    RecallFun fun;
};
class APP
{
private:
    ProtocolAnalysis *_msg;
    DRIVER_HANDLE control_handle = {"control", 1};
    uint32_t _seq = 0;
    uint8_t timeout = 100;

    std::vector<Node_INFO> _NodeList;
    std::list<UWB_D> _uwbdata;
    std::list<DRIVER_D> _driverdata;
    struct RES
    {
        uint32_t seq;
        int *code;
    };
    std::vector<RES> _respondlist;
    void clear_sqe(uint32_t seq);
    void setCode(int ack_code, uint32_t seq);
    MutexLock RecallLock;
    std::vector<ReCall> _Recall;
    void clearReCallSeq(uint32_t seq);
    int Send_CMD_toStateMachine(uint32_t seq, int CMD);
    void AddToRecv(int id, int *ok_flag, RecallFun DualFunction, void *ReturnValue);
    static int GetDoubleDataFunction(void *indata, void *returnvalue);
    static int GetLocationDataFunction(void *indata, void *returnvalue);

public:
    APP(ProtocolAnalysis *msg) : _msg(msg){};
    void run();
    void _Callback(ReturnFrameData in);

private:
    void _Callback_Get(ReturnFrameData in);
    void _Callback_Set(ReturnFrameData in);
    void _Callback_ACK(ReturnFrameData in);
    void _Callback_HEARBEAT(ReturnFrameData in);
    void reaction_ACK(void *, uint16_t seq);

public:
    //==========================GetData==API================================
    //读取数据
    int GetData(_data::LOCATION_DATA *returnvalue, UINT timeout);
    int GetData(int type, double *returnvalue, UINT timeout);
    int Set_Forward_motor(UINT8 mode, double volue, uint16_t timeout);
    int Set_Acc_motor(UINT8 mode, double volue, uint16_t timeout);
    int Set_Lift_motor(UINT8 mode, double volue, uint16_t timeout);
    int Set_Side_motor(UINT8 mode, double volue, uint16_t timeout);
    int Set_Turn_motor(UINT8 mode, double volue, uint16_t timeout);
    int Set_Brake(UINT8 mode, double volue, uint16_t timeout);
    int Set_AUTO(bool volue);
    enum MODE
    {
        mode_abs = 1,
        mode_inc = 0,
    };

private:
    //control

    int setControl(uint8_t mode, double value, uint16_t timeout, uint16_t type);
    int SendContrl(uint16_t type, double value, int timeout);
};
#endif