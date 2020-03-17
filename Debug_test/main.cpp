/*
 * @Description: 状态机节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-17 12:01:26
 * @LastEditors: Yaodecheng
 **/
#include "APP/app.h"
#include "sleep.h"
#include "pathcontrol/pathControler.h"
void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
APP app(&msgtest);

int fps;
void *timer0(void *cnt)
{
  int *f = (int *)cnt;
  while (1)
  {
    _sleep_ms(1000);
    //printf("fps=%d\n", *f);
    fps = *f;
    *f = 0;
  }
}
void *timer50(void *L)
{
  MutexLock *l = (MutexLock *)L;
  while (1)
  {
    l->unlock();
    Sleep(1000);
  }
}
int main()
{
  app.run();
  msgtest.init(Debug_port);
  uint64_t i = 0;
  int cnt = 0;
  thread_base fp(timer0, &cnt);
  MutexLock L;
  thread_base tim(timer50, &L);

  pathControler ctrl(&app);
  ctrl.control_loop();
  

    
  while (1)
  {
    L.lock();
    //double t;
    //struct timeval t1,t2;
    //gettimeofday(&t1, NULL);
    //_data::LOCATION_DATA Uwb;
    //int code = app.GetData(&Uwb,5);
   // gettimeofday(&t2, NULL);
    //printf("time=%d\n",((time_t)t2.tv_sec * (time_t)1000000 + (time_t)t2.tv_usec)-((time_t)t1.tv_sec * (time_t)1000000 + (time_t)t1.tv_usec));
    
    Sleep(1000);
    //printf("%s  %f  %f\n",printf_status(code).c_str(),Uwb.x,Uwb.y);
    //cnt++;
    //printf("%s==%f      %d   fps=%d\n",printf_status(code).c_str(),t,cnt++,fps);
  }
  return 0;
}

//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
  app._Callback(in);
}
