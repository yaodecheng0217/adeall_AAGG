/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-04-22 18:41:20
 * @LastEditors: Yaodecheng
 **/
#include "udpinterface/ProtocolAnalysis.h"
#include "response.h"

using namespace response;
using namespace msgpa;
using namespace std;
void Callback_outdata(ReturnFrameData in);
ProtocolAnalysis msgtest(Callback_outdata);
response_client client(&msgtest);

int main()
{
    msgtest.init(51087);
    neb::CJsonObject data;
    while (1)
    {
        bool online = true;
        int code = client.get_data_continuously("test", data, "127.0.0.1", 51086, online);
        printf("return code =%d\n", code);
        if (code == 0)
        {
            
            while (online)
            {
                Sleep(100);
               printf("recv data\n%s\n", data.ToFormattedString().c_str());
            }
        }
        printf("SERVER IS OFFLINE..\n");
        Sleep(1000);
    }
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    client.Reaction_Callback(in);
}
