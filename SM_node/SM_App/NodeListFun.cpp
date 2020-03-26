/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:12:38
 * @LastEditors: Yaodecheng
 */
#include "app.h"
#include "CJson/cJSON.h"
#include "CJson/CJsonObject.hpp"
bool APP::update(uint32_t driver_id, LOCATION_DATA *data)
{
    if (UpdateDataDetail(driver_id, *data))
    {
        clearonliecount(LOCATION, driver_id); //清零超时
        return 1;
    }
    return 0;
}
bool APP::update(TYPE_DOUBLE_UPDATE_DATA xx)
{
    if (UpdateDataDetail(xx.id, xx.data))
    {
        clearonliecount(DOUBLE_DATA, xx.id); //清零超时
        return 1;
    }
    return 0;
}
bool APP::update(uint32_t driver_id, ETV_DRIVER_STATE_DATA *data)
{
    if (UpdateDataDetail(driver_id, *data))
    {
        clearonliecount(ETV_Driver, driver_id); //清零超时
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
            switch (_NodeList[i].handle.data_type)
            {
            case LOCATION:
                ClearUWBData(_NodeList[i].handle.driver_id);
                break;
            case ETV_Driver:
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
        x.onlinecnt = 0;

        switch (x.handle.data_type)
        {
        case type_double:
        {
            V_double info;
            info.data.resize(x.handle.data_size);
            info.driver_name = x.handle.driver_name;
            _doubledatalist.push_back(info);
        }
        break;
        case type_bool:
        {

            V_bool info;
            info.data.resize(x.handle.data_size);
            info.driver_name = x.handle.driver_name;
            _booldatalist.push_back(info);
        }
        break;
        default:
            break;
        }
        _NodeList.push_back(x);
        printf("Add node ok  %s\n", x.handle.driver_name.c_str());
    }
}
void APP::print_Node_INOF(Node_INFO info)
{
    printf("name=%s driver_id=%d type=%d  %s  %d\n", info.handle.driver_name.c_str(), info.handle.driver_id, info.handle.data_type, info.ip.c_str(), info.port);
    switch (info.handle.data_type)
    {
    case type_bool:
    {
        size_t listsize = _doubledatalist.size();
        int loc = -1;
        for (size_t i = 0; i < listsize; i++)
        {
            if (_doubledatalist[i].driver_name == info.handle.data_name)
            {
                loc = i;
                break;
            }
        }
        if (loc == -1)
            break;

        neb::CJsonObject oJson(info.handle.data_name);
        std::cout << "data:";
        size_t count = oJson.GetArraySize();
        for (size_t i = 0; i < count; i++)
        {
            std::string name;
            oJson.Get(i, name);
            std::cout << "\n"
                      << name.c_str() << _doubledatalist[loc].data[i];
        }
        std::cout << std::endl;
    }
    break;
    case type_double:
    {

       size_t listsize = _booldatalist.size();
        int loc = -1;
        for (size_t i = 0; i < listsize; i++)
        {
            if (_doubledatalist[i].driver_name == info.handle.data_name)
            {
                loc = i;
                break;
            }
        }
        if (loc == -1)
            break;

        neb::CJsonObject oJson(info.handle.data_name);
        std::cout << "data:";
        size_t count = oJson.GetArraySize();
        for (size_t i = 0; i < count; i++)
        {
            std::string name;
            oJson.Get(i, name);
            std::cout << "\n"
                      << name.c_str() << _booldatalist[loc].data[i];
        }
        std::cout << std::endl;
    }
    break;
    //other type....
    default:
        break;
    }
}
bool APP::GetDataDetail(uint32_t id, LOCATION_DATA *data)
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
bool APP::GetDataDetail(uint32_t id, ETV_DRIVER_STATE_DATA *data)
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
bool APP::GetDataDetail(uint32_t id, double *data)
{
    for (DOUBLE_D x : _doubledata)
    {
        if (x.id == id)
        {
            *data = x.data;
            return 1;
        }
    }
    return 0;
}
bool APP::UpdateDataDetail(uint32_t driver_id, LOCATION_DATA data)
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
bool APP::UpdateDataDetail(uint32_t driver_id, ETV_DRIVER_STATE_DATA data)
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
bool APP::UpdateDataDetail(uint32_t driver_id, double data)
{
    std::vector<DOUBLE_D>::iterator iter;
    for (iter = _doubledata.begin(); iter != _doubledata.end(); iter++)
    {
        if (iter->id == driver_id)
        {
            iter->data = data;
            return 1;
        }
    }
    return 0;
}