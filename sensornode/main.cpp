/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-28 11:40:00
 * @LastEditors: Yaodecheng
 **/

#include "SensorApp/uwb_node.h"
//#include "vrep/vrep_interface.h"
#include"uwb/Senser.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
uwb_node app(&msgtest);
//vrep_interface vr;
UWBsenser uwb;
int main()
{  
    uwb.star("COM6","COM2");
    msgtest.init(Sensor_port);
    app.run();
    app._data.x = 1.11111;
    while (1)
    {
        //vrep_data d = vr.GetAllData();
        //printf("%f  %f  %f  %f  %f\n", d.Uwb_x, d.Uwb_y, d.Uwb_yaw * 57.3,d.side_lasser,d.TrayH_lasser);
        app.updateX(uwb.localsenser_data.Posx);
        app.updatey(uwb.localsenser_data.Posy);
        app.updatez(uwb.localsenser_data.Posz);
        //printf("-->%f  %f  %f\n", app._data.x,app._data.y,app._data.yaw);
        Sleep(50);
        //system("cls");
        //app.printfNodeInfo();
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
