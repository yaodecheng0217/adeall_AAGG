/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-04-07 19:09:04
 * @LastEditors: Yaodecheng
 **/

#include "SensorApp/uwb_node.h"
#include "DriverApp/ETV_driver.h"
#include "vrep/vrep_interface.h"
#include "SensorApp/lasser_node.h"
#include <math.h>
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
uwb_node app(&msgtest);
lasser_node laser(&msgtest, "laser", Type_TrayL_lasser);
vrep_interface vr;
ETV_driver driver(&msgtest, 1, &vr);
int main()
{
    vr.init();
    msgtest.init(Sensor_port);
    app.run();
    driver.run();
    laser.run();
    while (1)
    {
        vrep_data d = vr.GetAllData();

        //printf("%f  %f  %f  %f  %f\n", d.Uwb_x, d.Uwb_y, d.Uwb_yaw * 57.3,d.side_lasser,d.TrayH_lasser);
        double x = d.Uwb_x + 30 * cos(d.Uwb_yaw) ;//+ rand() % 20 - 10;
        double y = d.Uwb_y + 30 * sin(d.Uwb_yaw);// + rand() % 20 - 10;
        double speed=d.Accelerator_speed;
        app.updata(x, y, d.Uwb_yaw);
        driver.updateSpeed(speed);
        laser.updata(d.TrayL_lasser, d.TrayH_lasser, d.high_lasser, d.forward_lasser, d.side_lasser);
        //printf("%.2f $\n",d.Accelerator_speed*57.3);
        Sleep(50);
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
    driver._Callback(in);
    laser._Callback(in);
}
