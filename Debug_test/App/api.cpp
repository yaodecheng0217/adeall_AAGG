/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 13:59:39
 * @LastEditors: Yaodecheng
 */
#include "app.h"
int APP::Send_CMD_toStateMachine(uint32_t seq, int type)
{
    _Send::TYPE_GET_DATA aa;
    aa.handle = control_handle;
    aa.type = type;
    aa.seq = seq;
    return _msg->sendData("127.0.0.1",
                          StateMachine_port,
                          ID_Controller,
                          INS_GET,
                          CMD_GET_DATA,
                          aa);
}
void APP::AddToRecv(int seq, int *ok_flag, RecallFun DualFunction, void *ReturnValue)
{
    ReCall dual;
    dual.seq = seq;
    dual.flag = ok_flag;
    dual.fun = DualFunction; //处理函数
    dual.out = ReturnValue;
    _Recall.push_back(dual);
}
int APP::GetDoubleDataFunction(void *indata, void *returnvalue)
{
    _Send::TYPE_ACK_ONE_DATA d = *(_Send::TYPE_ACK_ONE_DATA *)indata;
    double *u = (double *)returnvalue;
    *u = d.data;
    return d.code;
}
int APP::GetLocationDataFunction(void *indata, void *returnvalue)
{
    _Send::TYPE_ACK_LOCATION_DATA d = *(_Send::TYPE_ACK_LOCATION_DATA *)indata;
    _data::LOCATION_DATA *u = (_data::LOCATION_DATA *)returnvalue;
    *u = d.data;
    return d.code;
}
int APP::GetData(_data::LOCATION_DATA *returnvalue, UINT timeout)
{
    uint32_t seq = _seq++;
    if (Send_CMD_toStateMachine(seq, _data::Type_location) == -1)
    {
        return ERR;
    }
    //加入待处理队列
    int ok_flag = 0;

    AddToRecv(seq, &ok_flag, GetLocationDataFunction, returnvalue);

    //超时等待
    while (timeout--)
    {
        if (ok_flag != 0)
        {
            if (ok_flag < 0)
                return ERR;
            else
                return OK;
        }
        Sleep(1);
    }
    clearReCallSeq(seq);
    return TIMEOUT;
}
int APP::GetData(int type, double *returnvalue, UINT timeout)
{
    uint32_t seq = _seq++;
    if (Send_CMD_toStateMachine(seq, type) == -1)
    {
        return ERR;
    }
    //加入待处理队列
    int ok_flag = -1;
    AddToRecv(seq, &ok_flag, GetDoubleDataFunction, returnvalue);
    //超时等待
    while (timeout--)
    {
        if (ok_flag != -1)
        {
            return ok_flag;
        }
        Sleep(1);
    }
    clearReCallSeq(seq);
    return TIMEOUT;
}
void APP::clearReCallSeq(uint32_t seq)
{
    size_t s = _Recall.size();
    for (size_t i = 0; i < s; i++)
    {
        if (_Recall[i].seq == seq)
        {
            _Recall.erase(_Recall.begin() + i);
            break;
        }
    }
}

void APP::reaction_ACK(void *r, uint16_t seq)
{
    size_t l = _Recall.size();
    for (size_t i = 0; i < l; i++)
    {
        //printf("=====%d===%d===\n",_Recall[i].seq,seq);
        if (_Recall[i].seq == seq)
        {
            *_Recall[i].flag = _Recall[i].fun(r, _Recall[i].out);
            _Recall.erase(_Recall.begin() + i);
            return;
        }
    }
}

int APP::SendContrl(uint16_t type, double value, int timeout)
{
    timeval tv;
    gettimeofday(&tv, NULL);
    _Send::TYPE_SET_DOUBLE_DATA setdata;
    setdata.handle = control_handle;
    setdata.data.type = type;
    setdata.data.value = value;
    setdata.seq = _seq++;
    setdata.timestamp = (time_t)tv.tv_sec * (time_t)1000000 + (time_t)tv.tv_usec;

    //wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = setdata.seq;
    _respondlist.push_back(w);
    for (size_t j = 0; j < 3; j++)
    {
        _msg->sendData("127.0.0.1",
                       PORT_LIST::StateMachine_port,
                       SOURCE_ID_LIST::ID_Controller,
                       INS_LIST::INS_SET,
                       CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA,
                       setdata);
        for (size_t i = 0; i < 50; i++)
        {
            if (code != -1)
            {
                clear_sqe(w.seq);
                return code;
            }
            Sleep(1);
        }
        //printf("%d retry %d times,\n", w.seq, j + 1);
    }

    printf("time out ------------------------%d\n", w.seq);
    clear_sqe(w.seq);
    return TIMEOUT;
}

int APP::Forward_motor(UINT8 mode, double value, uint16_t timeout)
{
    if (mode == MODE::mode_abs)
    {
        //发送新值
        int code = SendContrl(_data::Type_MoveForwardValue, value, timeout);
        return code;
    }
    else
    {
        double v;
        int code = GetData(_data::Type_MoveForwardValue, &v, 10);
        if (code == OK)
        {
            //发送新值
            //printf("Get value ok \n");
             code = SendContrl(_data::Type_MoveForwardValue, v+value, timeout);
            return code;
        }
        return code;
    }

    return ERR;
}

// int APP::Setfuntion(ModeValue set, uint8_t type, ContrlInterface * p)
// {
//     if (set.Mode == MODE::mode_abs)
//     {
//         //发送新值
//         int code = p->SendContrl(type, set.value, 100);
//         if (code == OK)
//         {
//             printf("Set [%d] ok!\n", type);
//             set.Look.unlock();
//             return 0;
//         }
//         else if (code == TIMEOUT)
//         {
//             printf("Set [%d] timeout!\n", type);
//             set.Look.unlock();
//             return 0;
//         }
//         else if (code == ERR)
//         {
//             printf("Set [%d] ERROR!\n", type);
//             set.Look.unlock();
//             return 0;
//         }
//     }
//     else if (set.Mode == MODE::mode_inc)
//     {
//         //printf("DDDDDDD\n");
//         //读取当前状态
//         float _value;
//         int code = p->ReturnState(type, &_value, 100);

//         if (code == OK)
//         {
//             //根据当前状态累加
//             _value = _value + set.value;
//             //发送新值
//             code = p->SendContrl(type, _value, 100);
//             if (code == OK)
//             {
//                 printf("Set [%d] ok!\n", type);
//                 set.Look.unlock();
//                 return 0;
//             }
//             else if (code == TIMEOUT)
//             {
//                 printf("Set [%d] timeout!\n", type);
//                 set.Look.unlock();
//                 return 0;
//             }
//             else if (code == ERR)
//             {
//                 printf("Set [%d] ERROR!\n", type);
//                 set.Look.unlock();
//                 return 0;
//             }
//         }
//         else if (code == ERR)
//         {
//             printf("Read [%d] value ERROR!\n", type);
//             set.Look.unlock();
//             return 0;
//         }
//         else if (code == TIMEOUT)
//         {
//             printf("Read [%d] value TIMEOUT!\n", type);
//             set.Look.unlock();
//             return 0;
//         }
//     }
//     //等待结果返回
//     set.Look.unlock();
//     return 0;
// }