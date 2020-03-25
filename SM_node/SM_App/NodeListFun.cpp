/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:12:38
 * @LastEditors: Yaodecheng
 */
#include "app.h"

bool APP::update(uint32_t driver_id,LOCATION_DATA *data)
{
    if (UpdateDataDetail(driver_id, *data))
    {
        clearonliecount(LOCATION,driver_id); //清零超时
        return 1;
    }
       return 0;
}
bool APP::update(TYPE_DOUBLE_UPDATE_DATA xx)
{
    if (UpdateDataDetail(xx.id,xx.data))
    {
        clearonliecount(DOUBLE_DATA,xx.id); //清零超时
        return 1;
    }
       return 0;
}
bool APP::update(uint32_t driver_id, ETV_DRIVER_STATE_DATA *data)
{
    if (UpdateDataDetail(driver_id, *data))
    {
        clearonliecount(ETV_Driver,driver_id); //清零超时
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
            switch (_NodeList[i].handle.driver_type)
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
        x.onlinecnt=0;
        switch (x.handle.driver_type)
        {
        case LOCATION:
        {
            UWB_D data;
            data.id = x.handle.driver_id;
            _uwbdata.push_back(data);
            printf("A-------------%d  %d\n", data.id, x.handle.driver_id);
        }
        break;
        case ETV_Driver:
        {
            DRIVER_D data;
            data.id = x.handle.driver_id;
            _driverdata.push_back(data);
            printf("B-------------%d  %d\n", data.id, x.handle.driver_id);
        }
        break;
         case DOUBLE_DATA:
        {
            DOUBLE_D data;
            data.id = x.handle.driver_id;
            _doubledata.push_back(data);
            printf("C-------------%d  %d\n", data.id, x.handle.driver_id);
        }
        break;
        default:
            break;
        }
        _NodeList.push_back(x);
        printf("Add ok  %d   %d \n", x.handle.driver_id, _uwbdata.size());
    }
}
void APP::print_Node_INOF(Node_INFO info)
{
    printf("name=%s driver_id=%d type=%d  %s  %d\n", info.handle.driver_name.c_str(), info.handle.driver_id, info.handle.driver_type, info.ip.c_str(), info.port);
    switch (info.handle.driver_type)
    {
    case LOCATION:
    {

         LOCATION_DATA d;
        if (GetDataDetail(info.handle.driver_id, &d))
        {
            printf("data:\n  x=%f,\n  y=%f,\n  z=%f,\n  yaw=%f\n", d.x, d.y, d.z, d.yaw);
        }
    }
    break;
    case ETV_Driver:
    {

        ETV_DRIVER_STATE_DATA d;
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
     case DOUBLE_DATA:
    {

         double d;
        if (GetDataDetail(info.handle.driver_id, &d))
        {
            printf("data:\n  %f,\n", d);
        }
    }
    break;
    //other type....
    default:
        break;
    }
}
bool APP::GetDataDetail(uint32_t id,LOCATION_DATA *data)
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
bool APP::UpdateDataDetail(uint32_t driver_id,ETV_DRIVER_STATE_DATA data)
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
bool APP::UpdateDataDetail(uint32_t driver_id,double data)
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