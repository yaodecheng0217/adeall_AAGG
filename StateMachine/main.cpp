/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-04-09 15:05:35
 * @LastEditors: Yaodecheng
 **/
#include "SM_App/app.h"
#include "sleep.h"
#include "readconfig/readjson.h"

void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest);
using namespace std;
int main()
{
    Readconfig init("Initconfig.json");
    init.setitem("common");
    int port=9001;
    bool print=false;
    init.GetValue("statemachine_port",port);
    init.setitem("statemachine");
    init.GetValue("printf_node_list",print);
    app.run();
    msgtest.init(port);
    while (1)
    {
        _sleep_ms(1000);
        if(print)
        app.print_Node_List();
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
