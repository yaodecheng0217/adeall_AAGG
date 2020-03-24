/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-24 10:58:56
 * @LastEditors: Yaodecheng
 **/
#include "APP/app.h"
#include "sleep.h"
#include "pathcontrol/pathControler.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest);
uint32_t ff=0,pp=0;;
void *timee(void *)
{
  while (1)
  {
    Sleep(1000);
    pp=ff;
    printf("%d\n",pp);
    ff=0;
  }
  
  return 0;
}
void *test2(void *)
{
return 0;
}
int main()
{
  app.run();
  msgtest.init(Debug_port);
  pathControler ctrl(&app);
  ctrl.control_loop();
  LOCATION_DATA uwb;
  thread_base t(timee,NULL);
  while (true)
  {
    int code = app.Set_Acc_motor(app.mode_abs, 0, 15);;
    //Sleep(500);
    //thread_base t2(test2,NULL);
    ff++;
    //printf("code=%s  %d\n", printf_status(code).c_str(),pp);
  }

  return 0;
}

//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
  app._Callback(in);
}
