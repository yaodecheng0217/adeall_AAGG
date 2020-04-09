/*
 * @Author: Yaodecheng
 * @Date: 2020-03-24 15:09:30
 * @LastEditTime: 2020-04-09 18:23:50
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-25 18:03:37
 * @LastEditors: Yaodecheng
 **/
#include "APP/app.h"
#include "sleep.h"
#include "pathcontrol/pathControler.h"
#include "keyboard.h"
#include "time_util.h"
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
  StarKeyboard();
  ctrl.control_loop4();
  while (true)
  {

    LOCATION_DATA loc;
    int code = app.GetData(&loc,50);
    printf("code=%s  %f  %f   %f\n", printf_status(code).c_str(),loc.x,loc.y,loc.yaw*57.3);
    Sleep(500);
    ff++;
    printf("=======\n");
  }

  return 0;
}

//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
  app._Callback(in);
}
