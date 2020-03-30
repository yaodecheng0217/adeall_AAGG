/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-30 15:54:15
 * @LastEditors: Yaodecheng
 **/

#include "SensorApp/sensor_node.h"
#include "udpinterface/ProtocolAnalysis.h"
#include "SensorApp/LaserInterface.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
LaserSensor app(&msgtest);

void update(INT32 x, INT32 y, INT32 yaw)
{
    double yawT=yaw/1000.0;
    yawT=yawT+180-15;
    if(yawT>360)
    yawT=yawT-360;

    app.updata(x/10.0,y/10.0,yawT/57.3);
}
UINT8 zl_LMDetect[36] = {0xFF, 0xAA, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x00, 0x00, 0x00, 0x04, 0x01, 0x06, 0x00, 0x00, 0x00, 0x26, 0xEE, 0xEE}; //导航模式指令
UINT8 zl_GetLocatinData[34] = {0xFF, 0xAA, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x06, 0x08, 0x00, 0x00,
                               0x00,
                               0x00, 0x00, 0x00,
                               0x00, 0x17, 0xEE, 0xEE}; //要数指令
int main()
{
    msgtest.init(Sensor_port);
    SetCallback(update);
    app.SetSendingInterval(100);
    NetConnectPara Curr_NetConnectPara;
    HWND m_hWnd;
    Curr_NetConnectPara.IP = ntohl(inet_addr("192.168.0.2"));
    Curr_NetConnectPara.Port = 2110;
    bool b_Connect = ConnectServer(m_hWnd, &Curr_NetConnectPara);
    if (b_Connect)
    {
        printf("conneted!\n");
        app.run();
        Curr_NetConnectPara.Senddata = (char *)zl_LMDetect;
        Curr_NetConnectPara.Len_Senddata = 36;
        SendData(&Curr_NetConnectPara);
        Sleep(1000);
        while (1)
        {
            //printf("=====>\n");
            Curr_NetConnectPara.Senddata = (char *)zl_GetLocatinData;
            Curr_NetConnectPara.Len_Senddata = 34;
            SendData(&Curr_NetConnectPara);
            Sleep(150);
        }
    }
    else
    {
        printf("ERROR\n");
    }
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
