/*
 * @Description: 传感器节点
 * @Author: Yaodecheng
 * @Date: 2019-10-09 09:08:07
 * @LastEditTime: 2020-03-23 19:57:17
 * @LastEditors: Yaodecheng
 **/
#include "DriverApp/ETV_driver.h"
#include "sleep.h"
#include"udpinterface/thread_base.h"
#include <iostream>
#include <cstdlib>
using namespace std;
#include <Windows.h>
#include <process.h>


void Callback_outdata(ReturnFrameData in);
msgpa::ProtocolAnalysis msgtest(Callback_outdata);
ETV_driver app(&msgtest, 12);

unsigned __stdcall Fun1Proc(LPVOID lpParam)
{
 cout<<"_beginthreadex create thread"<<endl;
 _endthread();
 return 0;
}
void *thread(void *a)
{
    Sleep(1000);
    cout<<"_beginthreadex create thread"<<*(size_t*)a<<endl;
    //ExitThread(0); // 这个函数是否调用都无所谓
   
    return 0;
}
int main()
{
    //msgtest.init(9999);
    //Sleep(1000);
    //app.initdata();
    //app.run();
    //uint32_t i=0;
    for(size_t i=0;i<10000;i++)
    {
        thread_base trr(thread,&i);
       //CreateThread(  0, 0, (LPTHREAD_START_ROUTINE)thread, 0, 0, 0 );
        //app.sendHandle(0);
        //FrameDataStruct xx;
        //msgtest.sendData("127.0.0.1",9001,xx);
        Sleep(100);
        //printf("%d\n",i++);
        //_sleep_ms(50);
    }
    while (true)
    {
        Sleep(1000);
        /* code */
    }
    
    return 0;
}
//正确收到数据后会调用此函数进行数据解包
void Callback_outdata(ReturnFrameData in)
{
    app._Callback(in);
}
