/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditTime: 2020-03-26 18:39:43
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
        Sleep(10);
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
void APP::recvAckCode(int ack, uint32_t seq)
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
int APP::waitForACK(uint32_t seq, int *code, uint32_t timeout)
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
void APP::SensorRsp(const char *ip, int prot, uint32_t seq,int code)
{
    //回应非序列化
    TYPE_ACK_CODE aa;
    aa.code = code;
    aa.seq = seq;
    //printf("%d\n", seq);
    _msg->sendData(ip,
                   prot,
                   ID_StateMachine,
                   INS_ACK,
                   CMD_TYPE_LIST::CMD_ACK_CODE,
                   aa);
}
void *APP::GetNodeData(DRIVER_HANDLE handle)
{
    size_t cnt = _NodeList.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_name == handle.driver_name && _NodeList[i].handle.driver_id == handle.driver_id)
        {
            return &_NodeList[i];
        }
    }
    return NULL;
}

void APP::ClearUWBData(uint32_t id)
{
    ScopeLocker K(&uwb_lock);
    std::vector<UWB_D>::iterator iter;
    for (iter = _uwbdata.begin(); iter != _uwbdata.end(); iter++)
    {
        printf("%d  %d\n", iter->id, id);
        if (iter->id == id)
        {

            printf("delet uwb id=%d\n", iter->id);
            _uwbdata.erase(iter);
            return;
        }
    }
    printf("delet uwb error=%d\n", iter->id);
}
void APP::ClearDriverData(uint32_t id)
{
    ScopeLocker K(&dri_lock);
    std::vector<DRIVER_D>::iterator iter;
    for (iter = _driverdata.begin(); iter != _driverdata.end(); iter++)
    {
        printf("%d  %d\n", iter->id, id);
        if (iter->id == id)
        {
            printf("delet dire id=%d\n", iter->id);
            _driverdata.erase(iter);
            return;
        }
    }
    printf("delet dire errr=%d\n", iter->id);
}
void APP::ClearDoubleData(uint32_t id)
{
    ScopeLocker K(&double_lock);
    std::vector<DOUBLE_D>::iterator iter;
    for (iter = _doubledata.begin(); iter != _doubledata.end(); iter++)
    {
        //printf("%d  %d\n", iter->id, id);
        if (iter->id == id)
        {
            printf("delet double id=%d\n", iter->id);
            _doubledata.erase(iter);
            return;
        }
    }
    printf("delet double error=%d\n", iter->id);
}
void APP::clear_sqe(uint32_t seq)
{
    ScopeLocker K(&_rslist_lock);
    size_t c = _respondlist.size();
    for (size_t i = 0; i < c; i++)
    {
        if (_respondlist[i].seq == seq)
        {
            _respondlist.erase(_respondlist.begin() + i);
            return;
        }
    }
    printf("delet file=======%d==========33333333333333333333333333333333333333333333333333333333333333333333\n", seq);
}
//打印缓存数据
void APP::printf__RecallList()
{
    printf("=========>>>\n");

    for (size_t i = 0; i < _respondlist.size(); i++)
    {
        printf("     -%d--%d--\n", i, _respondlist[i].seq);
    }

    for (size_t i = 0; i < _uwbdata.size(); i++)
    {
        printf("     ---%d--\n", _uwbdata[i].id);
    }

    for (size_t i = 0; i < _driverdata.size(); i++)
    {
        printf("     ***%d--\n", _driverdata[i].id);
    }
}
void APP::clearonliecount(int type, uint32_t driver_id)
{
    size_t size = _NodeList.size();
    for (size_t i = 0; i < size; i++)
    {
        Node_INFO *info = &_NodeList[i];
        if (info->handle.driver_id == driver_id && info->handle.driver_type == type)
        {
            info->onlinecnt = 0;
            return;
        }
    }
}
//        梅花
//     作者：王安石
//墙角数枝梅，凌寒独自开。
//遥知不是雪，为有暗香来
