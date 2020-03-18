/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-18 15:24:36
 * @LastEditors: Yaodecheng
 **/
#include "DriverApp/app.h"
#include "sleep.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest, 12);
int main()
{
    msgtest.init(9999);
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
