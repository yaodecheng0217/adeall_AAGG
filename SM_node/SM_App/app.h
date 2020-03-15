/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:15
 * @LastEditTime: 2020-03-15 22:45:42
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

struct Node_INFO
{
    DRIVER_HANDLE handle;
    char *ip;
    int port;
    void *data;
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
class APP
{
private:
    ProtocolAnalysis *_msg;
    DRIVER_HANDLE StateMachine = {"StateMachine", 1};
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
    void setCode(uint32_t ack, uint32_t seq);
public:
    APP(ProtocolAnalysis *msg) : _msg(msg){};
    void run();
    void _Callback(ReturnFrameData in);

    void print_Node_List();
    void print_Node_INOF(Node_INFO info);
    void SensorRsp(const char *ip, int prot, uint32_t seq);

private:
    void AddNodeList(DRIVER_HANDLE handle, char *ip, int port);
    void *GetNodeData(DRIVER_HANDLE handle);
    void update(DRIVER_HANDLE handle, void *data);
    void TimeUpdate();
    static void *loop10ms(void *);
    void ClearDriverData(uint8_t id);
    void ClearUWBData(uint8_t id);
    void * ETV_DriverOnlineChack();
    int sendToDriver(uint8_t type,double value);
public:
    //设置油门值,取值范围(1，-1)
    int SetAcceleratorValue(double value);
    //设置刹车值,取值范围(1,0)
    int SetBrake(double value);
    //设置转向角,取值范围(pi,-pi)
    int SetTurnAngle(double angle);
    //设置举升值，取值范围(1,-1)
    int SetLift(double value);
    //设置举侧移，取值范围(1,-1)
    int SetSide(double value);
    //设置前移  ，取值范围(1,-1)
    int SetMoveForward(double value);
    //设置倾斜值，取值范围(1,-1)
    int SetTilt(double value);
    //设置绿色Led
    int SetLedGreen(bool value);
    //设置红色Led
    int SetLedRed(bool value);
    //设置手刹
    int SetPacking(bool value);
    //设置自动
    int SetAuto(bool value);
};
#endif