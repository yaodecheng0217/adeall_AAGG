/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-15 13:03:02
 * @LastEditors: Yaodecheng
 **/
#include "SensorApp/app.h"
#include <cereal/archives/binary.hpp>
#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp" //一定要包含此文件，否则无法将std::string序列化为二进制形式，请看：https://github.com/USCiLab/cereal/issues/58

void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest, 1);
int main()
{
    msgtest.init(Sensor_port);
    app.run();
    app._data.x = 1.11111;
    while (1)
    {
#ifdef _WIN32
        Sleep(50);
#else
        usleep(1000 * 1000);
#endif
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
