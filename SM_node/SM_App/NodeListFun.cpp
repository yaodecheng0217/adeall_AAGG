/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:12:38
 * @LastEditors: Yaodecheng
 */
#include "app.h"
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
        _NodeList.push_back(x);
        printf("Add node ok  %s\n", x.handle.driver_name.c_str());
    }
}
void APP::print_Node_INOF(Node_INFO info)
{
    printf("name=%s driver_id=%d type=%d  %s  %d\n", info.handle.driver_name.c_str(), info.handle.driver_id, info.handle.data_type, info.ip.c_str(), info.port);
    printf("data:\n%s\n", info.data.ToFormattedString().c_str());
}
neb::CJsonObject *APP::GetDataDetail(std::string driver_name, std::string dataname)
{
    size_t cnt = _NodeList.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_name == driver_name)
        {
            return &_NodeList[i].data[dataname];
        }
    }
    return NULL;
}

bool APP::UpdateDataDetail(neb::CJsonObject newdata)
{
    size_t cnt = _NodeList.size();
    std::string driver_name;
    newdata.GetKey(driver_name);
    for (size_t i = 0; i < cnt; i++)
    {
        if (_NodeList[i].handle.driver_name == driver_name)
        {
            _NodeList[i].data = newdata[driver_name];
            _NodeList[i].onlinecnt = 0;
            return 1;
        }
    }
    return 0;
}
