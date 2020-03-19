/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-18 16:29:54
 * @LastEditors: Yaodecheng
 **/

#include "SensorApp/app.h"
#include "DriverApp/driver.h"
#include "vrep/vrep_interface.h"
void Callback_outdata(ReturnFrameData in);
void Callback_outdata2(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
msgpa::ProtocolAnalysis msgtest2(Callback_outdata2);
APP app(&msgtest, 1);
vrep_interface vr;
DRIVER driver(&msgtest2,1,&vr);
int main()
{  
    vr.init();
    msgtest.init(Sensor_port);
    app.run();
    msgtest2.init(Driver_port);
    driver.run();
    vr.Set_Turn_motor(90/57.3);
    vr.Set_Acc_motor(0.1);
    while (1)
    {
        vrep_data d = vr.GetAllData();
        //printf("%f  %f  %f  %f  %f\n", d.Uwb_x, d.Uwb_y, d.Uwb_yaw * 57.3,d.side_lasser,d.TrayH_lasser);
        app._data.x=d.Uwb_x;
        app._data.y=d.Uwb_y;
        app._data.yaw=d.Uwb_yaw;
        Sleep(50);
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata2(ReturnFrameData in)
{
    driver._Callback(in);
}
