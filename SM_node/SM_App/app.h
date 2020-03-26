/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:15
 * @LastEditTime: 2020-03-26 22:35:44
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
#include "CJson/cJSON.h"
#include "CJson/CJsonObject.hpp"

#ifndef _APP_H_
#define _APP_H_

using namespace msgpa;
using namespace adeall;

struct Node_INFO
{
    DRIVER_HANDLE handle;
    neb::CJsonObject data;
    std::string ip;
    int port;
    time_t timestamp;
    int onlinecnt;
};
struct V_double
{
    std::string driver_name;
    std::vector<double> data;
};
struct V_bool
{
    std::string driver_name;
    std::vector<bool> data;
};
struct UWB_D
{
    uint32_t id;
    LOCATION_DATA data;
};
struct DRIVER_D
{
    uint32_t id;
    ETV_DRIVER_STATE_DATA data;
};
struct DOUBLE_D
{
    uint32_t id;
    double data;
};
class APP
{
private:

    MutexLock vd_lock;
    std::vector<V_double> _doubledatalist;
    std::vector<V_bool> _booldatalist;


    ProtocolAnalysis *_msg;
    DRIVER_HANDLE StateMachine = {"StateMachine", 1};
    uint32_t _seq = 0;
    uint8_t timeout = 100;
    MutexLock info_lock;
    std::vector<Node_INFO> _NodeList;
    MutexLock uwb_lock;
    std::vector<UWB_D> _uwbdata;
    MutexLock dri_lock;
    std::vector<DRIVER_D> _driverdata;
    MutexLock double_lock;
    std::vector<DOUBLE_D> _doubledata;
    struct RES
    {
        uint32_t seq;
        int *code;
    };
    MutexLock _rslist_lock;
    std::vector<RES> _respondlist;
    void clear_sqe(uint32_t seq);
    int waitForACK(uint32_t seq, int *code, uint32_t timeout);
    void recvAckCode(int ack, uint32_t seq);
    void SensorRsp_N(const char *ip, int prot, uint32_t seq);
public:
    APP(ProtocolAnalysis *msg) : _msg(msg){};
    void run();
    void _Callback(ReturnFrameData in);
    void print_Node_List();

public:
neb::CJsonObject *GetDataDetail(std::string driver_name, std::string dataname);
    
    bool UpdateDataDetail(neb::CJsonObject newdata);
    void print_Node_INOF(Node_INFO info);
    void SensorRsp(const char *ip, int prot, uint32_t seq,int code);
    void AddNodeList(DRIVER_HANDLE handle, char *ip, int port);
    void *GetNodeData(DRIVER_HANDLE handle);
    Node_INFO *GetNode_INFO(uint16_t  type,uint32_t driver_id);
    void update(DRIVER_HANDLE handle, void *data);
    void TimeUpdate();
    static void *loop10ms(void *);
    void ClearDriverData(uint32_t id);
    void ClearUWBData(uint32_t id);
    void ClearDoubleData(uint32_t id);
    bool GetDataDetail(std::string driver_name, std::vector<double> *data);
    bool GetDataDetail(std::string driver_name, std::vector<bool> *data);
    bool UpdateDataDetail(std::string updatedata);
    bool UpdateDataDetail(uint32_t driver_id, ETV_DRIVER_STATE_DATA data);
    bool UpdateDataDetail(uint32_t driver_id,double data);
    void *ETV_DriverOnlineChack();
    void *UWB_DriverOnlineChack();
    int sendToDriver(const char *ip, int port, uint8_t type, double value);
    bool update(uint32_t driver_id, LOCATION_DATA *data);
    bool update(uint32_t driver_id, ETV_DRIVER_STATE_DATA *data);
    bool update(TYPE_DOUBLE_UPDATE_DATA data);
    void clearonliecount(int type,uint32_t driver_id);
    int GetlistLoc(std::string driver_name,std::vector<V_double>list);
    int GetlistLoc(std::string driver_name,std::vector<V_bool>list);
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
    void printf__RecallList();

private:
    //=========================api==============
    void Get_Information(const char *ip, int port, uint16_t type, uint32_t seq);
    int set_ControlValue(uint16_t type, double value);
    void Set_ACK(const char *ip, int port, int code, uint32_t seq);
    void ACK_One_data(const char *ip, int port, uint16_t type, uint32_t seq);
    void _Callback_Get(ReturnFrameData in);
    void _Callback_Set(ReturnFrameData in);
    void _Callback_ACK(ReturnFrameData in);
    void _Callback_HEARBEAT(ReturnFrameData in);
};
#endif