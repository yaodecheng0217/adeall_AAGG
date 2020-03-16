/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditTime: 2020-03-16 16:28:00
 * @LastEditors: Yaodecheng
 */
#include "app.h"
void APP::run()
{
    thread_base T1(loop10ms, this);
}
void *APP::loop10ms(void *p)
{
    APP *a = (APP *)p;
    while (true)
    {
        a->TimeUpdate();
        Sleep(50);
    }
}
void APP::print_Node_List()
{
    printf("\n<<--------------\n");
    for (Node_INFO x : _NodeList)
    {
        print_Node_INOF(x);
    }
    printf("\n-------------->>\n");
}
void APP::setCode(uint32_t ack, uint32_t seq)
{
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
void APP::SensorRsp(const char *ip, int prot, uint32_t seq)
{
    //回应
    _Send::TYPE_HEARBEAT_ACK aa;
    aa.handle = StateMachine;
    aa.code = OK;
    aa.seq = seq;
    //printf("%d\n", seq);
    _msg->sendData(ip,
                   prot,
                   ID_StateMachine,
                   INS_ACK,
                   CMD_TYPE_LIST::CMD_ACK_HEARBEAT,
                   aa);
}

void *APP::GetNodeData(DRIVER_HANDLE handle)
{
    size_t cnt = _NodeList.size();

    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_name == handle.driver_name && _NodeList[i].handle.driver_id == handle.driver_id)
        {
            //printf("finded is %s\n", handle.driver_name.c_str());
            //print_Sensor_INOF(x);
            return &_NodeList[i];
        }
    }

    return NULL;
}

void APP::ClearUWBData(uint8_t id)
{
    std::list<UWB_D>::iterator iter;
    for (iter = _uwbdata.begin(); iter != _uwbdata.end(); iter++)
    {
        if (iter->id == id)
        {
            _uwbdata.erase(iter);
        }
    }
}
void APP::ClearDriverData(uint8_t id)
{
    std::list<DRIVER_D>::iterator iter;
    for (iter = _driverdata.begin(); iter != _driverdata.end(); iter++)
    {
        if (iter->id == id)
        {
            _driverdata.erase(iter);
        }
    }
}

void APP::clear_sqe(uint32_t seq)
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


//        梅花
//     作者：王安石
//墙角数枝梅，凌寒独自开。
//遥知不是雪，为有暗香来
