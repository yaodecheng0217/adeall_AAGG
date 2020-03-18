/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-18 18:08:26
 * @LastEditors: Yaodecheng
 **/
#include "SM_App/app.h"
#include "sleep.h"

void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest);
int main()
{
    app.run();
    msgtest.init(StateMachine_port);
    double value=0.0;
    while (1)
    {
        _sleep_ms(1000);
       //app.print_Node_List();

        value=value+0.1;
        //app.SensorRsp("127.0.0.1",9001,1);
    }
    return 0;
}

//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
        app._Callback(in);
}
