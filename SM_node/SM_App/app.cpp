/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditTime: 2020-03-15 22:45:22
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
void APP::print_Node_INOF(Node_INFO info)
{
    printf("name=%s driver_id=%d type=%d  %s  %d\n", info.handle.driver_name.c_str(), info.handle.driver_id, info.handle.datatype,info.ip,info.port);
    switch (info.handle.datatype)
    {
    case DATA_LIST::LOCATION:
    {
        _data::LOCATION_DATA d = *(_data::LOCATION_DATA *)info.data;
        printf("data:\n  x=%f,\n  y=%f,\n  z=%f,\n  yaw=%f\n", d.x, d.y, d.z, d.yaw);
    }
    break;
    case DATA_LIST::ETV_DriverState:
    {
        _data::ETV_DRIVER_STATE_DATA d = *(_data::ETV_DRIVER_STATE_DATA *)info.data;
        std::cout << "data:"
                  << "\n   AcceleratorValue=" << d.AcceleratorValue
                  << "\n   BrakeValue=" << d.BrakeValue
                  << "\n   TurnAngleValue=" << d.TurnAngleValue
                  << "\n   LiftValue=" << d.LiftValue
                  << "\n   SideValue=" << d.SideValue
                  << "\n   MoveForwardValue=" << d.MoveForwardValue
                  << "\n   TiltValue=" << d.TiltValue
                  << "\n   LED_Green=" << d.LED_Green
                  << "\n   LED_Red=" << d.LED_Red
                  << "\n   Paking=" << d.Paking
                  << "\n   AUTO=" << d.AUTO << std::endl;
    }
    break;
    //other type....
    default:
        break;
    }
}
void APP::_Callback(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_UWB_HEARBEAT_DATA:
    {
        _Send::TYPE_UWB_HEARBEAT_DATA xx;
        Decode_StructSerialize(&xx, in._databuff);

        AddNodeList(xx.handle,in.ip,in.prot);
        SensorRsp(in.ip, in.prot, xx.seq);
        update(xx.handle, &xx.data);
    }
    break;
    case CMD_TYPE_LIST::CMD_ETV_DRIVER_HEARBEAT_DATA:
    {
        _Send::TYPE_ETV_DRIVER_HEARBEAT_DATA xx;
        Decode_StructSerialize(&xx, in._databuff);

        AddNodeList(xx.handle,in.ip,in.prot);
        SensorRsp(in.ip, in.prot, xx.seq);
        update(xx.handle, &xx.data);
    }
    break;
    case CMD_TYPE_LIST::CMD_SET_ACK:
    {
         _Send::TYPE_SET_ACK r;
        Decode_StructSerialize(&r, in._databuff);
        setCode(r.code, r.seq);
    }
    break;
    default:
        break;
    }
}
void APP::setCode(uint32_t ack, uint32_t seq)
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
                   CMD_TYPE_LIST::CMD_HEARBEAT_ACK,
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
void APP::AddNodeList(DRIVER_HANDLE handle,char * ip,int port)
{
    static MutexLock LLL;
    if (GetNodeData(handle) == NULL)
    {
        ScopeLocker lock(&LLL);
        Node_INFO x;
        x.handle = handle;
        x.ip=ip;
        x.port=port;
        switch (x.handle.datatype)
        {
        case DATA_LIST::LOCATION:
        {
            UWB_D data;
            data.id=x.handle.driver_id;
            _uwbdata.push_back(data);
            x.data = &_uwbdata.back();
        }
        break;
        case DATA_LIST::ETV_DriverState:
        {
            DRIVER_D data;
            data.id=x.handle.driver_id;
            _driverdata.push_back(data);
            x.data = &_driverdata.back();
        }
        break;
        default:
            break;
        }
        _NodeList.push_back(x);
        printf("Add ok  %d   %d  %d\n", x.handle.driver_id, x.data, _uwbdata.size());
    }
}
void APP::update(DRIVER_HANDLE handle, void *data)
{
    Node_INFO *info = (Node_INFO *)GetNodeData(handle);
    if (info)
    {

        switch (info->handle.datatype)
        {
        case DATA_LIST::LOCATION:
        {
            //printf("%d---\n",info->handle.driver_id);
            info->onlinecnt = 0; //清零超时
            _data::LOCATION_DATA *dd = (_data::LOCATION_DATA *)data;
            _data::LOCATION_DATA *d = (_data::LOCATION_DATA *)info->data;
            *d = *dd;
        }
        break;
        case DATA_LIST::ETV_DriverState:
        {
            info->onlinecnt = 0; //清零超时
            _data::ETV_DRIVER_STATE_DATA *dd = (_data::ETV_DRIVER_STATE_DATA *)data;
            _data::ETV_DRIVER_STATE_DATA *d = (_data::ETV_DRIVER_STATE_DATA *)info->data;
            *d = *dd;
        }
        //other type....
        default:
            break;
        }
    }
    else
    {
        printf("No mach\n");
    }
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
void APP::TimeUpdate()
{
    size_t size = _NodeList.size();
    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        //printf("*--------------------------------------------------------------------------------------------%d\n",_SensorList[i - j].onlinecnt);
        if (_NodeList[i - j].onlinecnt++ > 50)
        {
            size_t cnt = i - j;
            printf("%s %d sensor die!\n", _NodeList[cnt].handle.driver_name.c_str(), _NodeList[cnt].handle.driver_id);
            switch (_NodeList[cnt].handle.datatype)
            {
            case DATA_LIST::LOCATION:
            ClearUWBData(_NodeList[cnt].handle.driver_id);
            break;
            case DATA_LIST::ETV_DriverState:
            ClearDriverData(_NodeList[cnt].handle.driver_id);
            break;

            default:
                break;
            }
            _NodeList.erase(_NodeList.begin() + cnt);
            j++;
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
