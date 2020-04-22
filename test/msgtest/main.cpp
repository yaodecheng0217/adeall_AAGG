/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-04-22 18:29:17
 * @LastEditors: Yaodecheng
 **/
#include "udpinterface/ProtocolAnalysis.h"
#include "response.h"

using namespace response;
using namespace msgpa;
using namespace std;
void Callback_outdata(ReturnFrameData in);
ProtocolAnalysis msgtest(Callback_outdata);
response_server server(&msgtest);
neb::CJsonObject test(void *)
{static int x=0;
    printf("test server return a data is ok!\n");
    neb::CJsonObject d;
    d.Add("a data", 123);
    d.Add("x", x++);
    return d;
}
int main()
{
    msgtest.init(51086);
    server.Add_reaction("test", test, NULL);
    bool online = true;
    while (1)
    {
        Sleep(100);
    }
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    server.Reaction_Callback(in);
}
