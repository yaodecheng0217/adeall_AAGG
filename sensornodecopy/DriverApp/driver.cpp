/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditors: Yaodecheng
 */
#include "driver.h"
#include "thread_base.h"
void DRIVER::send(){};

void DRIVER::run()
{
    //创建一个线程向指定节点发送数据
    thread_base t1(hearbeat_thread, this);
    _driver_state_data.AUTO = 1;
}

void DRIVER::clear_sqe(uint32_t seq)
{
    size_t c = _respondlist.size();
    for (size_t i = 0; i < c; i++)
    {
        if (_respondlist[i].seq == seq)
        {
            _respondlist.erase(_respondlist.begin() + i);
            break;
        }
    }
}
void DRIVER::hearbeat_loop()
{
    static int cnt = 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    _Send::TYPE_ETV_DRIVER_HEARBEAT_DATA hearbeat;
    hearbeat.handle = Driver_handle;
    hearbeat.data = _driver_state_data;
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
        _msg->sendData(SM_ip.c_str(),
                       PORT_LIST::StateMachine_port,
                       SOURCE_ID_LIST::ID_Sensor_uwb,
                       INS_LIST::INS_HARBEAT,
                       CMD_TYPE_LIST::CMD_HEARBEAT_ETV_DRIVER_DATA,
                       hearbeat);
        for (size_t i = 0; i < 50; i++)
        {
            if (code != -1)
            {
                clear_sqe(w.seq);
                Sleep(4);
                cnt = 0;
                return;
            }
            Sleep(10);
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
void *DRIVER::hearbeat_thread(void *is)
{
    DRIVER *p = (DRIVER *)is;
    while (true)
    {
        p->hearbeat_loop();
    }
    return 0;
}
void DRIVER::_Callback(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_ACK_HEARBEAT:
    {
        _Send::TYPE_HEARBEAT_ACK r;
        Decode_StructSerialize(&r, in._databuff);
        setCode(r.code, r.seq);
    }
    break;
    case CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA:
    {
        _Send::TYPE_SET_DOUBLE_DATA r;
        Decode_StructSerialize(&r, in._databuff);
        int code = setDoubleValue(r.data.type, r.data.value);
        printf("------#%d\n",code);
        set_ACK_send(in.ip, in.port, code, r.seq);
    }
    break;
    break;
    default:
        break;
    }
}
void DRIVER::setCode(uint32_t ack, uint32_t seq)
{
    size_t count = _respondlist.size();
    for (size_t i = 0; i < count; i++)
    {
        if (_respondlist[i].seq == seq)
        {
            *_respondlist[i].code = ack;
            //printf("ack!!  %d\n",*_respondlist[i].code);
            break;
        }
    }
}

void DRIVER::set_ACK_send(const char *ip, int prot, int code, uint32_t seq)
{
    //回应
    _Send::TYPE_SET_ACK aa;
    aa.handle = Driver_handle;
    aa.code = code;
    aa.seq = seq;
    //printf("%d\n", seq);
    _msg->sendData(ip,
                   prot,
                   ID_StateMachine,
                   INS_ACK,
                   CMD_TYPE_LIST::CMD_ACK_SET,
                   aa);
}

int DRIVER::setDoubleValue(uint8_t type, double value)
{
    switch (type)
    {
    case _data::DATA_TYPE_LIST::Type_AcceleratorValue:
        _driver_state_data.AcceleratorValue = value;
        vr->Set_Acc_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_BrakeValue:
        _driver_state_data.BrakeValue = value;
        break;
    case _data::DATA_TYPE_LIST::Type_LiftValue:
        _driver_state_data.LiftValue = value;
        vr->Set_Lift_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_MoveForwardValue:
        _driver_state_data.MoveForwardValue = value;
        vr->Set_Forward_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_SideValue:
        _driver_state_data.SideValue = value;
        vr->Set_Side_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_TiltValue:
        _driver_state_data.TiltValue = value;
        break;
    case _data::DATA_TYPE_LIST::Type_TurnAngleValue:
        _driver_state_data.TurnAngleValue = value;
        vr->Set_Turn_motor(value);
        break;
    case _data::DATA_TYPE_LIST::Type_AUTO:
        _driver_state_data.AUTO = value;
        break;
    case _data::DATA_TYPE_LIST::Type_LED_Green:
        _driver_state_data.LED_Green = value;
        break;
    case _data::DATA_TYPE_LIST::Type_LED_Red:
        _driver_state_data.LED_Red = value;
        break;
    case _data::DATA_TYPE_LIST::Type_Paking:
        _driver_state_data.Paking = value;
        break;
    default:
        return ERR;
        break;
    }
    return OK;
}
int DRIVER::setBoolValue(uint8_t type, bool value)
{
    return OK;
}