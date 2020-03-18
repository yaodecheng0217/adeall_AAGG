/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditors: Yaodecheng
 */
#include "app.h"
#include "thread_base.h"
void APP::send(){};

void APP::run()
{
    //创建一个线程向指定节点发送数据
    thread_base t1(hearbeat_thread, this);
}

void APP::clear_sqe(uint32_t seq)
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
void APP::hearbeat_loop()
{
    static int cnt = 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    _Send::TYPE_UWB_HEARBEAT_DATA hearbeat;
    hearbeat.handle = uwb_handle;
    hearbeat.data=_data;
    hearbeat.seq = _seq++;
    hearbeat.timestamp = (time_t)tv.tv_sec * (time_t)1000000 + (time_t)tv.tv_usec;

    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = hearbeat.seq;
    _respondlist.push_back(w);
    //printf("%d\n", w.seq);
    for (size_t j = 0; j < 3; j++)
    {
        _msg->sendData("127.0.0.1",
                       StateMachine_port,
                       ID_Sensor_uwb,
                       INS_LIST::INS_HARBEAT,
                       CMD_TYPE_LIST::CMD_HEARBEAT_UWB_DATA,
                       hearbeat);
        for (size_t i = 0; i < 50; i++)
        {
            Sleep(1);
            if (code != -1)
            {
                clear_sqe(w.seq);
                Sleep(50);
                cnt = 0;
                return;
            }
        }
        //printf("%d retry %d times,\n", w.seq, j + 1);
    }

    printf("time out ------------------------%d\n", w.seq);
    clear_sqe(w.seq);
    cnt++;
    if (cnt > 10)
    {
        Sleep(1000);
    }
}
void *APP::hearbeat_thread(void *is)
{
    APP *p = (APP *)is;
    while (true)
    {
        p->hearbeat_loop();
    }
    return 0;
}
void APP::_Callback(ReturnFrameData in)
{   
    if (CMD_TYPE_LIST::CMD_ACK_HEARBEAT == in.cmd_type)
    {
        _Send::TYPE_HEARBEAT_ACK r;
        Decode_StructSerialize(&r, in._databuff);
        size_t count = _respondlist.size();
        for (size_t i = 0; i < count; i++)
        {
            if (_respondlist[i].seq == r.seq)
            {
                *_respondlist[i].code = r.code;
                //printf("ack!!  %d\n",*_respondlist[i].code);
                break;
            }
        }
    }
}