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
    return _msg->sendData("127.0.0.1", StateMachine_port, ID_Controller, INS_GET,
                          CMD_GET_DATA, aa);
}
void APP::AddToRecv(int seq, int *ok_flag, RecallFun DualFunction,
                    void *ReturnValue)
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
    int code = -1;
    AddToRecv(seq, &code, GetLocationDataFunction, returnvalue);
    if (Send_CMD_toStateMachine(seq, _data::Type_location) == -1)
    {
        clearReCallSeq(seq);
        return ERR;      
    }
    //超时等待
    while (timeout--)
    {
         Sleep(1);
        if (code != -1)
        {
            return code;
        } 
    }
    clearReCallSeq(seq);
    printf("get data timeout\n");
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
    int code = -1;
    AddToRecv(seq, &code, GetDoubleDataFunction, returnvalue);
    //超时等待
    while (timeout--)
    {Sleep(1);
        if (code != -1)
        {
            return code;
        }
        
    }
    clearReCallSeq(seq);
    return TIMEOUT;
}
void APP::clearReCallSeq(uint32_t seq)
{
    ScopeLocker K(&RecallLock);
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
     ScopeLocker K(&RecallLock);
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
     
    _msg->sendData("127.0.0.1", PORT_LIST::StateMachine_port,
                       SOURCE_ID_LIST::ID_Controller, INS_LIST::INS_SET,
                       CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA, setdata);

                       return OK;//==========================todo


    // wait for ack
    int code = -1;
    RES w;
    w.code = &code;
    w.seq = setdata.seq;
    _respondlist.push_back(w);
    for (size_t j = 0; j < 3; j++)
    {
        _msg->sendData("127.0.0.1", PORT_LIST::StateMachine_port,
                       SOURCE_ID_LIST::ID_Controller, INS_LIST::INS_SET,
                       CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA, setdata);
        for (size_t i = 0; i < 50; i++)
        {
            if (code != -1)
            {
                clear_sqe(w.seq);
                return code;
            }
            Sleep(1);
        }
        // printf("%d retry %d times,\n", w.seq, j + 1);
    }
    printf("time out ------------------------%d\n", w.seq);
    clear_sqe(w.seq);
    return TIMEOUT;
}
int APP::setControl(uint8_t mode, double value, uint16_t timeout,
                    uint16_t type)
{
    if (mode == MODE::mode_abs)
    {
        int code = SendContrl(type, value, timeout);
        return code;
    }
    else
    {
        double v;
        int code = GetData(type, &v, 15);
        if (code == OK)
        {
            code = SendContrl(type, v + value, timeout);
            return code;
        }
        return code;
    }

    return ERR;
}
int APP::Set_Forward_motor(UINT8 mode, double value, uint16_t timeout = 10)
{
    return setControl(mode, value, timeout, _data::Type_MoveForwardValue);
}

int APP::Set_Acc_motor(UINT8 mode, double value, uint16_t timeout = 10)
{
    return setControl(mode, value, timeout, _data::Type_AcceleratorValue);
}
int APP::Set_Lift_motor(UINT8 mode, double value, uint16_t timeout = 10)
{
    return setControl(mode, value, timeout, _data::Type_LiftValue);
}
int APP::Set_Side_motor(UINT8 mode, double value, uint16_t timeout = 10)
{
    return setControl(mode, value, timeout, _data::Type_SideValue);
}
int APP::Set_Turn_motor(UINT8 mode, double value, uint16_t timeout = 10)
{
    return setControl(mode, value, timeout, _data::Type_TurnAngleValue);
}
int APP::Set_Brake(UINT8 mode, double value, uint16_t timeout = 10)
{
    return setControl(mode, value, timeout, _data::Type_BrakeValue);
}
int APP::Set_AUTO(bool value)
{
    return setControl(MODE::mode_abs, value, 100, _data::Type_AUTO);
}