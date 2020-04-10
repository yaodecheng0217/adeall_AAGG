/*
 * @Author: Yaodecheng
 * @Date: 2020-03-24 15:09:30
 * @LastEditTime: 2020-04-10 18:51:02
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
#include "readconfig/readjson.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest);
uint32_t ff = 0, pp = 0;
;
void *timee(void *)
{
  while (1)
  {
    Sleep(1000);
    pp = ff;
    printf("%d\n", pp);
    ff = 0;
  }

  return 0;
}
void *test2(void *)
{
  return 0;
}
// 参数1和参数2分别是前景的颜色和背景的颜色。 
// 
// 参数1可选择一下值： 
//// FOREGROUND_BLUE                Text color contains blue. 
//// FOREGROUND_GREEN                Text color contains green. 
//// FOREGROUND_RED                    Text color contains red. 
//// FOREGROUND_INTENSITY            Text color is intensified. 
// 
// 参数2可选择一下值： 
//// BACKGROUND_BLUE                Background color contains blue. 
//// BACKGROUND_GREEN                Background color contains green. 
//// BACKGROUND_RED                    Background color contains red. 
//// BACKGROUND_INTENSITY            Background color is intensified. 
bool SetConsoleColor( WORD forceGroundColor, WORD backGroundColor )
{ 
    // 获得缓冲区句柄。参数可以选择一下三种值： 
    //// STD_INPUT_HANDLE            标准输入的句柄 
    //// STD_OUTPUT_HANDLE            标准输出的句柄 
    //// STD_ERROR_HANDLE`            标准错误的句柄 
    HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE ); 
    if( handle == 0 ) 
        return false; 

    //设置文本及背景颜色。 
    BOOL ret = SetConsoleTextAttribute( handle, forceGroundColor ); 
    return( ret == TRUE ); 
}

int main()
{
   SetConsoleColor( FOREGROUND_GREEN, BACKGROUND_INTENSITY );
   std::cout<<"hello"<<std::endl;
  app.run();
  msgtest.init(Debug_port);
  pathControler ctrl(&app);
  StarKeyboard();

  while (1)
  {
    std::cout << GLOG_TIMESTAMP(GetCurrentTime());
    Readconfig set("Initconfig.json");
    int lo = 2;
    double speed = 25;
    set.setitem("debug");
    set.GetValue("loop", lo);
    set.GetValue("speed", speed);
    switch (lo)
    {
    case 1:
      ctrl.control_loop(speed);
      break;
    case 2:
      ctrl.control_loop2(speed);
      break;
    case 3:
      ctrl.control_loop3(speed);
      break;
    case 4:
      ctrl.control_loop4(speed);
      break;
    default:
      Sleep(1000);
      break;
    }
  }
  while (true)
  {

    LOCATION_DATA loc;
    int code = app.GetData(&loc, 50);
    printf("code=%s  %f  %f   %f\n", printf_status(code).c_str(), loc.x, loc.y, loc.yaw * 57.3);
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
