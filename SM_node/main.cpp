/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-15 22:52:40
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

        //设置油门值,取值范围(1，-1)
        app.SetAuto(1);
      int rs = app.SetAcceleratorValue( value);
      printf("rscode=%d",rs);
    //设置刹车值,取值范围(1,0)
     app.SetBrake( value);
    //设置转向角,取值范围(pi,-pi)
     app.SetTurnAngle( value);
    //设置举升值，取值范围(1,-1)
     app.SetLift( 0);
    //设置举侧移，取值范围(1,-1)
     app.SetSide( value);
    //设置前移  ，取值范围(1,-1)
     app.SetMoveForward( value);
    //设置倾斜值，取值范围(1,-1)
     app.SetTilt( value);
    //设置绿色Led
     app.SetLedGreen( value);
    //设置红色Led
     app.SetLedRed( value);
    //设置手
     app.SetPacking( value);
    //设置自动
     app.SetAuto( value);
       app.print_Node_List();

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
