/******************************************************************************
 * Copyright 2019 The Adeall Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
/*
 * @Author: Yaodecheng
 * @Date: 2020-04-21 16:26:36
 * @LastEditTime: 2020-04-22 18:22:56
 * @LastEditors: Yaodecheng
 * @Description: response function
 */

#pragma once
#include "udpinterface/ProtocolAnalysis.h"
#include "define_msg_data.h"
namespace response
{
using namespace msgpa;
using namespace adeall;
typedef neb::CJsonObject (*action_function)(void *);
struct Action
{
    std::string actionname;
    void *_conte;
    action_function callbackFunction = NULL;
};
struct RES
{
    uint32_t seq;
    int code = -1;
    neb::CJsonObject info;
};
struct AutoReq
{
    Action action;
    const char *ip;
    int port;
    void * context;
    MutexLock Lock;
};
class response_server
{

private:
    MutexLock _actionlist_lock;
    std::vector<Action> _action_list;

    MutexLock _rslist_lock;
    std::vector<RES *> _respondlist;
    ProtocolAnalysis *_msg;
    void Reaction(uint8_t type, neb::CJsonObject info, const char *ip, int port);
    void Response_a_data(std::string name, uint32_t seq, const char *ip, int port);
    void Response_data_continuously(std::string name, uint32_t seq, const char *ip, int port);
    static void *AutoReqFun(void *p);

public:
    response_server(ProtocolAnalysis *msg);
    ~response_server();
    //添加响应
    int Add_reaction(std::string name, action_function callback, void *context);
    //关闭响应接口
    int Remove_reaction(std::string name);
    //正确收到数据后会调用此函数进行数据解包
    void Reaction_Callback(ReturnFrameData in);
};

class response_client
{
    struct AUTODATA
    {
        std::string name;
        neb::CJsonObject *data;
        bool *online;
        uint8_t timecount = 0;
    };

private:
    ProtocolAnalysis *_msg;
    MutexLock _rslist_lock;
    std::vector<RES *> _respondlist;
    void Reaction(uint8_t type, neb::CJsonObject info, const char *ip, int port);
    std::vector<AUTODATA> _autodatalist;
    void add_to_autoupdate(std::string nane, bool &onlone, neb::CJsonObject &data);
    void update_autodata(std::string name, neb::CJsonObject data, const char *ip, int port, uint32_t seq);
    static void *loop_50ms(void *);
    void count_up();
    void delet_autoupdate(std::string name);

public:
    response_client(ProtocolAnalysis *msg);
    int get_once_data(std::string dataname, neb::CJsonObject &info, const char *ip, int port, int timeout);
    int get_data_continuously(std::string dataname, neb::CJsonObject &info, const char *ip, int port, bool &online);
    ~response_client();
    void Reaction_Callback(ReturnFrameData in);
};
} // namespace response