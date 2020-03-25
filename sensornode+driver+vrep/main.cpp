/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-25 17:06:17
 * @LastEditors: Yaodecheng
 **/

#include "SensorApp/uwb_node.h"
#include "DriverApp/ETV_driver.h"
#include "vrep/vrep_interface.h"
#include "SensorApp/lasser_node.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
uwb_node app(&msgtest);
lasser_node TrayL_lasser(&msgtest,"TrayL_lasser",Type_TrayL_lasser), 
TrayH_lasser(&msgtest,"TrayH_lasser", Type_TrayH_lasser), 
high_lasser(&msgtest,"high_lasser",Type_high_lasser), 
forward_lasser(&msgtest,"forward_lasser",Type_forward_lasser), 
side_lasser(&msgtest,"side_lasser",Type_side_lasser);
vrep_interface vr;
ETV_driver driver(&msgtest, 1, &vr);
int main()
{
    vr.init();
    msgtest.init(Sensor_port);
    app.run();
    driver.run();
    TrayL_lasser.run();
    TrayH_lasser.run();
    high_lasser.run();
    forward_lasser.run();
    side_lasser.run();

    vr.Set_Turn_motor(90 / 57.3);
    vr.Set_Acc_motor(0.1);
    while (1)
    {
        vrep_data d = vr.GetAllData();

        //printf("%f  %f  %f  %f  %f\n", d.Uwb_x, d.Uwb_y, d.Uwb_yaw * 57.3,d.side_lasser,d.TrayH_lasser);
        app._data.x = d.Uwb_x;
        app._data.y = d.Uwb_y;
        app._data.yaw = d.Uwb_yaw;

        TrayL_lasser._data = d.TrayL_lasser;
        TrayH_lasser._data = d.TrayH_lasser;
        high_lasser._data = d.high_lasser;
        forward_lasser._data = d.forward_lasser;
        side_lasser._data = d.side_lasser;

        Sleep(50);
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
    driver._Callback(in);
    TrayL_lasser._Callback(in);
    TrayH_lasser._Callback(in);
    high_lasser._Callback(in);
    forward_lasser._Callback(in);
    side_lasser._Callback(in);
}
