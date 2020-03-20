/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:12:38
 * @LastEditors: Yaodecheng
 */
#include "app.h"
void APP::update(DRIVER_HANDLE handle, void *data)
{
    Node_INFO *info = (Node_INFO *)GetNodeData(handle);
    if (info)
    {
        switch (info->handle.datatype)
        {
        case DATA_LIST::LOCATION:
        {
            if (UpdateDataDetail(info->handle.driver_id, (*(_data::LOCATION_DATA *)data)))
                info->onlinecnt = 0; //清零超时
        }
        break;
        case DATA_LIST::ETV_DriverState:
        {
            if (UpdateDataDetail(info->handle.driver_id, (*(_data::ETV_DRIVER_STATE_DATA *)data)))
                info->onlinecnt = 0; //清零超时
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
bool APP::update(uint32_t driver_id, _data::LOCATION_DATA *data)
{
    if (UpdateDataDetail(driver_id, *data))
    {
        clearonliecount(DATA_LIST::LOCATION,driver_id); //清零超时
        return 1;
    }
       return 0;
}
void APP::TimeUpdate()
{
    ScopeLocker lock(&info_lock);
    size_t size = _NodeList.size();
    for (size_t i = 0; i < size; i++)
    {
        //printf("*--------------------------------------------------------------------------------------------%d\n",_NodeList[i].onlinecnt);
        if (_NodeList[i].onlinecnt++ > 10)
        {
            printf("%s %d sensor die!\n", _NodeList[i].handle.driver_name.c_str(), _NodeList[i].handle.driver_id);
            switch (_NodeList[i].handle.datatype)
            {
            case DATA_LIST::LOCATION:
                ClearUWBData(_NodeList[i].handle.driver_id);
                break;
            case DATA_LIST::ETV_DriverState:
                ClearDriverData(_NodeList[i].handle.driver_id);
                break;

            default:
                break;
            }
            _NodeList.erase(_NodeList.begin() + i);
        }
    }
}
void APP::AddNodeList(DRIVER_HANDLE handle, char *ip, int port)
{
    ScopeLocker lock(&info_lock);
    if (GetNodeData(handle) == NULL)
    {

        Node_INFO x;
        x.handle = handle;
        x.ip = ip;
        x.port = port;
        switch (x.handle.datatype)
        {
        case DATA_LIST::LOCATION:
        {
            UWB_D data;
            data.id = x.handle.driver_id;
            _uwbdata.push_back(data);
            printf("A-------------%d  %d\n", data.id, x.handle.driver_id);
        }
        break;
        case DATA_LIST::ETV_DriverState:
        {
            DRIVER_D data;
            data.id = x.handle.driver_id;
            _driverdata.push_back(data);
            printf("B-------------%d  %d\n", data.id, x.handle.driver_id);
        }
        break;
        default:
            break;
        }
        _NodeList.push_back(x);
        printf("Add ok  %d   %d  %d\n", x.handle.driver_id, x.data, _uwbdata.size());
    }
}
void APP::print_Node_INOF(Node_INFO info)
{
    printf("name=%s driver_id=%d type=%d  %s  %d\n", info.handle.driver_name.c_str(), info.handle.driver_id, info.handle.datatype, info.ip.c_str(), info.port);
    switch (info.handle.datatype)
    {
    case DATA_LIST::LOCATION:
    {

        _data::LOCATION_DATA d;
        if (GetDataDetail(info.handle.driver_id, &d))
        {
            printf("data:\n  x=%f,\n  y=%f,\n  z=%f,\n  yaw=%f\n", d.x, d.y, d.z, d.yaw);
        }
    }
    break;
    case DATA_LIST::ETV_DriverState:
    {

        _data::ETV_DRIVER_STATE_DATA d;
        if (GetDataDetail(info.handle.driver_id, &d))
        {
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
    }
    break;
    //other type....
    default:
        break;
    }
}
bool APP::GetDataDetail(uint32_t id, _data::LOCATION_DATA *data)
{
    for (UWB_D x : _uwbdata)
    {
        if (x.id == id)
        {
            *data = x.data;
            return 1;
        }
    }
    return 0;
}
bool APP::GetDataDetail(uint32_t id, _data::ETV_DRIVER_STATE_DATA *data)
{
    for (DRIVER_D x : _driverdata)
    {
        if (x.id == id)
        {
            *data = x.data;
            return 1;
        }
    }
    return 0;
}
bool APP::UpdateDataDetail(uint32_t driver_id, _data::LOCATION_DATA data)
{
    std::vector<UWB_D>::iterator iter;
    for (iter = _uwbdata.begin(); iter != _uwbdata.end(); iter++)
    {
        if (iter->id == driver_id)
        {
            iter->data = data;
            return 1;
        }
    }
    return 0;
}
bool APP::UpdateDataDetail(uint32_t driver_id, _data::ETV_DRIVER_STATE_DATA data)
{
    std::vector<DRIVER_D>::iterator iter;
    for (iter = _driverdata.begin(); iter != _driverdata.end(); iter++)
    {
        if (iter->id == driver_id)
        {
            iter->data = data;
            return 1;
        }
    }
    return 0;
}