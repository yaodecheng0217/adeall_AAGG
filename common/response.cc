/*
 * @Author: Yaodecheng
 * @Date: 2020-04-21 16:27:38
 * @LastEditTime: 2020-04-22 18:35:27
 * @LastEditors: Yaodecheng
 * @Description: response function
 */
#include "response.h"
#include "thread_base.h"
namespace response
{
//指令类型，仅在REACTION_API里面有效，不对外公开
enum CMD_type_LIST
{
    server_ack,
    client_ack,
    Request_once_data,
    Request_data_continuously,
    server_update_data,
};
uint32_t send_data(uint8_t type, neb::CJsonObject data, ProtocolAnalysis *_msg, const char *ip, int port)
{
    static uint32_t seq = 0;
    seq++;

    neb::CJsonObject oJson;
    oJson.Add("seq", seq);
    oJson.Add("data", data);

    _msg->sendStringData(ip,
                         port,
                         0,
                         INS_LIST::INS_REACTION_API,
                         type, //设置
                         oJson.ToString());
    //printf("%s\n",oJson.ToFormattedString().c_str());
    return seq;
}
Action find_in_action_list(std::string name, std::vector<Action> _list)
{
    Action null_action;
    size_t count = _list.size();
    for (size_t i = 0; i < count; i++)
    {
        if (_list[i].actionname == name)
        {
            return _list[i];
        }
    }
    return null_action;
}
void recvAckCode(MutexLock &lock, std::vector<RES *> &list, int ack, uint32_t seq, neb::CJsonObject info)
{
    ScopeLocker K(&lock);
    size_t count = list.size();
    for (size_t i = 0; i < count; i++)
    {
        if (list[i]->seq == seq)
        {
            list[i]->code = ack;
            list[i]->info = info;
            break;
        }
    }
}
void AddToACK(RES &w, MutexLock &lock, std::vector<RES *> &list)
{
    lock.lock();
    list.push_back(&w);
    lock.unlock();
}
int waitForACK(uint32_t seq, int *code, uint32_t timeout)
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
void clearSqe(uint32_t seq, std::vector<RES *> &list)
{
    size_t c = list.size();
    for (size_t i = 0; i < c; i++)
    {
        if (list[i]->seq == seq)
        {
            //printf("delet=seq=%d====%d\n",_respondlist[i].seq,seq);
            list.erase(list.begin() + i);
            break;
        }
    }
}
//==========================server=======================
response_server::response_server(ProtocolAnalysis *msg)
    : _msg(msg)
{
}

response_server::~response_server()
{
}
void response_server::Reaction_Callback(ReturnFrameData in)
{
    if (in.ins == INS_REACTION_API)
    {
        std::stringstream ss;
        for (uint8_t x : in._databuff)
            ss << x;
        neb::CJsonObject info(ss.str().c_str());

        Reaction(in.cmd_type, info, in.ip, in.port);
    }
}
void response_server::Reaction(uint8_t type, neb::CJsonObject info, const char *ip, int port)
{
    if (info.IsEmpty())
        return;

    uint32_t seq;
    info.Get("seq", seq);
    switch (type)
    {
    case client_ack:
    {
        int code = -1;
        uint32_t ack_seq = 0;
        neb::CJsonObject data;
        info["data"].Get("ack_seq", ack_seq);
        info["data"].Get("ack_code", code);
        info["data"].Get("ack_data", data);
        //printf("%s\n", info["data"].ToFormattedString().c_str());
        recvAckCode(_rslist_lock, _respondlist, code, ack_seq, data);
    }
    break;
    case Request_once_data:
    {
        std::string name;
        info["data"].Get("name", name);
        Response_a_data(name, seq, ip, port);
    }
    break;
    case Request_data_continuously:
    {
        std::string name;
        info["data"].Get("name", name);
        Response_data_continuously(name, seq, ip, port);
    }
    break;
    default:
        break;
    }
}
void response_server::Response_a_data(std::string name, uint32_t seq, const char *ip, int port)
{
    Action action = find_in_action_list(name, _action_list);
    if (action.callbackFunction != NULL)
    {
        neb::CJsonObject data;
        data.Add("ack_seq", seq);
        data.Add("ack_code", 1);
        data.Add("ack_data", action.callbackFunction(action._conte));
        send_data(server_ack, data, _msg, ip, port);
    }
    else
    {
        neb::CJsonObject data;
        data.Add("ack_seq", seq);
        data.Add("ack_code", 0);
        send_data(server_ack, data, _msg, ip, port);
    }
}
void *response_server ::AutoReqFun(void *p)
{
    AutoReq *req = (AutoReq *)p;
    AutoReq A = *req;
    req->Lock.unlock();
    response_server *server = (response_server *)A.context;
    int cnt = 0;
    while (1)
    {
        Sleep(50);
        neb::CJsonObject updata;
        updata.Add("name", A.action.actionname);
        updata.Add("update_data", A.action.callbackFunction(A.action._conte));
        int seq = send_data(server_update_data, updata, server->_msg, A.ip, A.port);
        RES w;
        w.seq = seq;
        AddToACK(w, server->_rslist_lock, server->_respondlist);
        if (waitForACK(seq, &w.code, 20))
        {
            cnt = 0;
        }
        else
        {
            cnt++;
        }
        clearSqe(seq, server->_respondlist);
        if (cnt > 10)
            break;
    }
    printf("client offline\n");
    return 0;
}
void response_server::Response_data_continuously(std::string name, uint32_t seq, const char *ip, int port)
{
    Action action = find_in_action_list(name, _action_list);
    if (action.callbackFunction != NULL)
    {
        neb::CJsonObject data;
        data.Add("ack_seq", seq);
        data.Add("ack_code", 1);
        data.Add("ack_data", action.callbackFunction(action._conte));
        send_data(server_ack, data, _msg, ip, port);
        //启动自动回复线程
        AutoReq req;
        req.action = action;
        req.ip = ip;
        req.port = port;
        req.context = this;
        req.Lock.lock();
        thread_base t1(AutoReqFun, &req);
        req.Lock.lock();
    }
    else
    {
        neb::CJsonObject data;
        data.Add("ack_seq", seq);
        data.Add("ack_code", 0);
        send_data(server_ack, data, _msg, ip, port);
    }
}
int response_server::Add_reaction(std::string name, action_function callback, void *context)
{
    Action action;
    action.actionname = name;
    action._conte = context;
    action.callbackFunction = callback;
    _action_list.push_back(action);
    return 1;
}
int response_server::Remove_reaction(std::string name)
{
    size_t count = _action_list.size();
    for (size_t i = 0; i < count; i++)
    {
        if (_action_list[i].actionname == name)
        {
            _action_list.erase(_action_list.begin() + i);
            return 1;
        }
    }
    return 0;
}
//=============================client=====================
response_client::response_client(ProtocolAnalysis *msg)
    : _msg(msg)
{
    thread_base t1(loop_50ms,this);
}

response_client::~response_client()
{
}
int response_client::get_once_data(std::string dataname, neb::CJsonObject &info, const char *ip, int port, int timeout)
{
    //0 正常获取到数据
    //1 获取数据超时
    //2 获取数据异常
    neb::CJsonObject data;
    data.Add("name", dataname);
    uint32_t seq = send_data(Request_once_data, data, _msg, ip, port);
    RES w;
    w.seq = seq;
    AddToACK(w, _rslist_lock, _respondlist);
    if (waitForACK(seq, &w.code, 50))
    {
        info = w.info;
        clearSqe(seq, _respondlist);
        return 0;
    }
    else
    {
        clearSqe(seq, _respondlist);
        return 1;
    }
    clearSqe(seq, _respondlist);
    return 2;
}
void response_client::Reaction_Callback(ReturnFrameData in)
{
    if (in.ins == INS_REACTION_API)
    {
        std::stringstream ss;
        for (uint8_t x : in._databuff)
            ss << x;
        neb::CJsonObject info(ss.str().c_str());

        Reaction(in.cmd_type, info, in.ip, in.port);
    }
}
void response_client::Reaction(uint8_t type, neb::CJsonObject info, const char *ip, int port)
{
    if (info.IsEmpty())
        return;
    //printf("%s\n",info.ToFormattedString().c_str());
    uint32_t seq;
    info.Get("seq", seq);
    switch (type)
    {
    case server_ack:
    {
        int code = -1;
        uint32_t ack_seq = 0;
        neb::CJsonObject data;
        info["data"].Get("ack_seq", ack_seq);
        info["data"].Get("ack_code", code);
        info["data"].Get("ack_data", data);
        //printf("%s\n",info["data"].ToFormattedString().c_str());
        recvAckCode(_rslist_lock, _respondlist, code, ack_seq, data);
    }
    case server_update_data:
    {
        neb::CJsonObject data;
        std::string name;
        info["data"].Get("name", name);
        info["data"].Get("update_data", data);
        //printf("%s\n",info.ToFormattedString().c_str());
        update_autodata(name, data, ip, port, seq);
    }
    break;
    default:
        break;
    }
}
int response_client::get_data_continuously(std::string dataname, neb::CJsonObject &info, const char *ip, int port, bool &online)
{
    //0 正常获取到数据
    //1 获取数据超时
    //2 获取数据异常
    neb::CJsonObject data;
    data.Add("name", dataname);
    uint32_t seq = send_data(Request_data_continuously, data, _msg, ip, port);
    RES w;
    w.seq = seq;
    AddToACK(w, _rslist_lock, _respondlist);
    if (waitForACK(seq, &w.code, 50))
    {
        info = w.info;
        online = true;
        clearSqe(seq, _respondlist);
        //正常获取，添加到自动维护列表
        add_to_autoupdate(dataname, online, info);
        return 0;
    }
    else
    {
        online = false;
        clearSqe(seq, _respondlist);
        return 1;
    }
    clearSqe(seq, _respondlist);
    return 2;
}
void response_client::update_autodata(std::string name, neb::CJsonObject data, const char *ip, int port, uint32_t seq)
{
    size_t count = _autodatalist.size();
    for (size_t i = 0; i < count; i++)
    {
        if (_autodatalist[i].name == name)
        {
            (*_autodatalist[i].data) = data;
            _autodatalist[i].timecount = 0;
            //回复服务器
            neb::CJsonObject data;
            data.Add("ack_seq", seq);
            data.Add("ack_code", 1);
            send_data(client_ack, data, _msg, ip, port);
            return;
        }
    }
}
void *response_client::loop_50ms(void *p)
{
    response_client *client = (response_client *)p;
    while (1)
    {
        Sleep(50);
        client->count_up();
    }
}
void response_client::count_up()
{
    size_t count = _autodatalist.size();
    for (size_t i = 0; i < count; i++)
    {
        _autodatalist[i].timecount++;
        if (_autodatalist[i].timecount > 10)
        {
            printf("%s offline\n", _autodatalist[i].name);
            *_autodatalist[i].online = false;
            delet_autoupdate(_autodatalist[i].name);
        }
    }
}
void response_client::delet_autoupdate(std::string name)
{
    size_t count = _autodatalist.size();
    for (size_t i = 0; i < count; i++)
    {
        if (_autodatalist[i].name == name)
        {
            _autodatalist.erase(_autodatalist.begin() + i);
            return;
        }
    }
}
void response_client::add_to_autoupdate(std::string name, bool &onlone, neb::CJsonObject &data)
{
    AUTODATA info;
    info.data = &data;
    info.name = name;
    info.online = &onlone;
    _autodatalist.push_back(info);
}
} // namespace response