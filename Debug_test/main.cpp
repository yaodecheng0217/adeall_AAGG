/*
 * @Author: Yaodecheng
 * @Date: 2020-03-24 15:09:30
 * @LastEditTime: 2020-04-03 14:42:58
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
  /*while(1)
  {
    Sleep(1);
  }*/
  ctrl.control_loop();
  //ctrl.Tracking_arc(300,0,0);
  ctrl.getup();
  Sleep(1000);
  ctrl.putdown();
 
  while (true)
  {

    LOCATION_DATA loc;
    int code = app.GetData(&loc,50);
    printf("code=%s  %f  %f   %f\n", printf_status(code).c_str(),loc.x,loc.y,loc.yaw*57.3);

    /*double data;
    code = app.GetData(DATA_SET_GET_TYPE_LIST::Type_forward_lasser,&data,50);;
    printf("code=%s  %f\n", printf_status(code).c_str(),data);
    code = app.GetData(DATA_SET_GET_TYPE_LIST::Type_high_lasser,&data,50);;
    printf("code=%s  %f\n", printf_status(code).c_str(),data);
    code = app.GetData(DATA_SET_GET_TYPE_LIST::Type_TrayH_lasser,&data,50);;
    printf("code=%s  %f\n", printf_status(code).c_str(),data);
    code = app.GetData(DATA_SET_GET_TYPE_LIST::Type_TrayL_lasser,&data,50);;
    printf("code=%s  %f\n", printf_status(code).c_str(),data);
    code = app.GetData(DATA_SET_GET_TYPE_LIST::Type_side_lasser,&data,50);;
    printf("code=%s  %f\n", printf_status(code).c_str(),data);*/
    Sleep(500);
    //thread_base t2(test2,NULL);
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
