/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditors: Yaodecheng
 */
#include "drivernode.h"
#include "thread_base.h"
#include "readconfig/readjson.h"
void Driver_node::_Callback(ReturnFrameData in)
{
    switch (in.ins)
    {
    case INS_ACK:
    {
        _Callback_ACK(in);
    }
    break;
    case INS_GET:
    {

        _Callback_Get(in);
    }
    break;
    case INS_SET:
    {
        _Callback_Set(in);
    }
    break;
    case INS_HARBEAT:
    {
        _Callback_HEARBEAT(in);
    }
    break;
    default:
        break;
    }
}
void Driver_node::_Callback_HEARBEAT(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    default:
        break;
    }
}
void Driver_node::_Callback_Get(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    default:
        break;
    }
}
void Driver_node::_Callback_Set(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA:
    {

        std::stringstream ss;
        for (uint8_t x : in._databuff)
        {
            ss << x;
        }
        neb::CJsonObject set(ss.str().c_str());

        uint32_t seq;
        std::string type;
        double value;
        set.Get("seq", seq);
        set.Get("type", type);
        set.Get("value", value);
        //printf("set %s %f seq=%d\n",type.c_str(),value,seq);
        set_ACK_send(in.ip, in.port, OK, seq); //设置函数用时过长===========先响应
        int code = setDoubleValue(type, value);
    }
    break;
    default:
        break;
    }
}
void Driver_node::_Callback_ACK(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_ACK_CODE:
    {
        TYPE_ACK_CODE r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        recvAckCode(r.code, r.seq);
    }
    break;
    default:
        break;
    }
}
void Driver_node::recvAckCode(int ack, uint32_t seq)
{
    ScopeLocker K(&_rslist_lock);
    size_t count = _respondlist.size();
    for (size_t i = 0; i < count; i++)
    {
        if (_respondlist[i].seq == seq)
        {
            *_respondlist[i].code = ack;
            break;
        }
    }
}
int Driver_node::waitForACK(uint32_t seq, int *code, uint32_t timeout)
{
    for (size_t i = 0; i < timeout; i++)
    {
        Sleep(1);
        if (*code != -1)
        {
            return 1;
        }
    }
    return 0;
}
void Driver_node::clearSqe(uint32_t seq)
{
    size_t c = _respondlist.size();
    for (size_t i = 0; i < c; i++)
    {
        if (_respondlist[i].seq == seq)
        {
            //printf("delet=seq=%d====%d\n",_respondlist[i].seq,seq);
            _respondlist.erase(_respondlist.begin() + i);
            break;
        }
    }
}
void Driver_node::sendDataLoop()
{

    if (_cnt > 10)
    {
        Sleep(1000);
        while (sendHandleAction() == 0)
        {
            Sleep(1000);
        }
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint32_t seq = _seq++;
    time_t timestamp = (time_t)tv.tv_sec * (time_t)1000000 + (time_t)tv.tv_usec;
    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = seq;
    _rslist_lock.lock();
    _respondlist.push_back(w);
    _rslist_lock.unlock();
    //printf("%d\n", w.seq);
    for (size_t j = 0; j < 3; j++)
    {
        sendData(seq, timestamp);
        if (waitForACK(w.seq, w.code, 20))
        {
            if (*w.code == ERR)
            {
                sendHandleAction();
            }
            _cnt = 0;
            clearSqe(w.seq);
            return;
        }
        //printf("%d retry %d times,\n", w.seq, j + 1);
    }
    printf("%s update time out %d\n", _handle.driver_name.c_str(), w.seq);
    clearSqe(w.seq);
    _cnt++;
}
void Driver_node::sendHandle(uint32_t seq)
{
    neb::CJsonObject oJson;
    TYPE_handle_string s;
    oJson.Add("driver_name", _handle.driver_name);
    oJson.Add("driver_id", _handle.driver_id);
    oJson.Add("data_type", _handle.data_type);
    oJson.Add("data_size", _handle.data_size);
    oJson.Add("data_list", _handle.data_list);
    oJson.Add("seq", seq);

    _msg->sendStringData(server_ip.c_str(),
                         server_port,
                         source_id,
                         INS_LIST::INS_HARBEAT,
                         CMD_TYPE_LIST::CMD_HEARBEAT_HANDLE, //设置
                         oJson.ToString());
}
void Driver_node::sendData(uint32_t seq, time_t timestamp)
{
    neb::CJsonObject oJson;
    oJson.Add("seq", seq);
    oJson.AddEmptySubObject("update");
    datalist_up();
    oJson["update"].Add(_handle.driver_name, _handle.data_list);

    _msg->sendStringData(server_ip.c_str(),
                         server_port,
                         source_id,
                         INS_LIST::INS_HARBEAT,
                         CMD_TYPE_LIST::CMD_UPDATE_DATA, //设置
                         oJson.ToString());
}
int Driver_node::sendHandleAction()
{
    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = _seq++;
    _rslist_lock.lock();
    _respondlist.push_back(w);
    _rslist_lock.unlock();
    //printf("%d\n", w.seq);
    //printf("Send handle !!\n");
    for (size_t j = 0; j < 3; j++)
    {
        sendHandle(w.seq);
        if (waitForACK(w.seq, w.code, 50))
        {
            if (*w.code == OK)
            {
                clearSqe(w.seq);
                printf("%s handle  ACK is ok ready to update!!\n", _handle.driver_name.c_str());
                return 1;
            }
            else
            {
                printf("%s handle  ACK is ERROR%d!!\n", _handle.driver_name.c_str(), *w.code);
                clearSqe(w.seq);
                return 0;
            }
        }
    }
    printf("%s Send handle time out %d\n", _handle.driver_name.c_str(), w.seq);
    clearSqe(w.seq);
    return 0;
}
void Driver_node::run()
{
    Readconfig config("initconfig.json");
    config.setitem("common");
    config.GetValue("statemachine_ip", server_ip);
    config.GetValue("statemachine_port", server_port);
    initdata();
    thread_base t0(timer, this);
    //创建一个线程向指定节点发送数据
    thread_base t1(hearbeatThread, this);
}
void *Driver_node::timer(void *is)
{
    Driver_node *p = (Driver_node *)is;
    while (true)
    {
        if (p->run_flag)
            p->_time_lock.unlock();
        Sleep(p->_SendingInterval);
    }
    return 0;
}
void *Driver_node::hearbeatThread(void *is)
{
    Driver_node *p = (Driver_node *)is;
    if (p->sendHandleAction())
    {
        p->_cnt = 0;
    }
    else
    {
        p->_cnt = 20;
    }
    while (true)
    {
        p->_time_lock.lock();
        p->sendDataLoop();
    }
    return 0;
}

void Driver_node::SetTimeout(uint16_t timeout)
{
    _timeout = timeout;
}
void Driver_node::set_ACK_send(const char *ip, int prot, int code, uint32_t seq)
{
    //回应
    TYPE_ACK_CODE aa;
    aa.code = code;
    aa.seq = seq;
    //printf("set -->%d\n", seq);
    _msg->sendData(ip,
                   prot,
                   0,
                   INS_ACK,
                   CMD_TYPE_LIST::CMD_ACK_SET,
                   aa);
}
void Driver_node::SetSendingInterval(uint16_t time)
{
    _SendingInterval = time;
}
void Driver_node::printfNodeInfo()
{
    printf("=====Node info:=====\nSM=%s %d\nSendingInterval=%dms\nnodeName=%s\ndatalist:\n%s\n",
           server_ip.c_str(),
           server_port,
           _SendingInterval,
           _handle.driver_name.c_str(),
           _handle.data_list.ToFormattedString().c_str());
}

void Driver_node::StopIs()
{
    run_flag = 0;
}
void Driver_node::Continue()
{
    run_flag = 1;
}