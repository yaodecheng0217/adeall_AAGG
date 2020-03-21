/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditors: Yaodecheng
 */
#include "sensornode.h"
#include "thread_base.h"
void Sensor_node::_Callback(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_ACK_HEARBEAT:
    {
        _Send::TYPE_HEARBEAT_ACK r;
        Decode_StructSerialize(&r, in._databuff);
        recvAckCode(r.code, r.seq);
    }
    break;
    case CMD_TYPE_LIST::N_CMD_ACK_HEARBEAT:
    {
        _Send::N_TYPE_HEARBEAT_ACK r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        recvAckCode(r.code, r.seq);
    }
    break;

    default:
        break;
    }
}
void Sensor_node::recvAckCode(uint32_t ack, uint32_t seq)
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
int Sensor_node::waitForACK(uint32_t seq, int *code, uint32_t timeout)
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
void Sensor_node::clearSqe(uint32_t seq)
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
void Sensor_node::sendDataLoop()
{
    static int cnt = 10;
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
        if (waitForACK(w.seq, w.code, _timeout))
        {
            cnt = 0;
            clearSqe(w.seq);
            return;
        }
        //printf("%d retry %d times,\n", w.seq, j + 1);
    }
    printf("update time out %d\n", w.seq);
    clearSqe(w.seq);
    cnt++;
    if (cnt > 10)
    {
        Sleep(1000);
        sendHandleAction();
    }
}

void Sensor_node::sendHandleAction()
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
    printf("Send handle !!\n");
    for (size_t j = 0; j < 3; j++)
    {
        sendHandle(w.seq);
        if (waitForACK(w.seq, w.code, _timeout))
        {
            clearSqe(w.seq);
            printf("handle  ACK is ok ready to update!!\n");
            return;
        }
    }
    printf("time out ------------------------%d\n", w.seq);
    clearSqe(w.seq);
}
void Sensor_node::run()
{
    initdata();
    thread_base t0(timer, this);
    //创建一个线程向指定节点发送数据
    thread_base t1(hearbeatThread, this);
}
void *Sensor_node::timer(void *is)
{
    Sensor_node *p = (Sensor_node *)is;
    while (true)
    {
        p->time_lock.unlock();
        Sleep(1000 / p->Frequency);
    }
    return 0;
}
void *Sensor_node::hearbeatThread(void *is)
{
    Sensor_node *p = (Sensor_node *)is;
    p->sendHandleAction();
    while (true)
    {
        p->time_lock.lock();
        p->sendDataLoop();
    }
    return 0;
}

void Sensor_node::SetTimeout(uint16_t timeout)
{
  _timeout=timeout;
}
void Sensor_node::SetFrequency(uint16_t hz)
{
  Frequency=hz;
}