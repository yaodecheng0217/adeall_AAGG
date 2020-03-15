/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-15 12:06:33
 * @LastEditors: Yaodecheng
 **/
#include "DriverApp/app.h"
#include "sleep.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest, 1);
int main()
{
    msgtest.init(Driver_port);
    app.run();
    while (1)
    {
        _sleep_ms(50);
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
