/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-03-30 15:08:29
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "ETV_driver.h"
#include "udpinterface/thread_base.h"
#include "output.h"




APP_name::APP_name(ProtocolAnalysis *msg) : Driver_node(msg)
{
}
APP_name::~APP_name()
{
}
void *APP_name::controlOnline(void *etv)
{
    APP_name *p = (APP_name *)etv;
    while (true)
    {
        if (p->Control_count > 5)
        {
            p->_data.acc=0;
            p->_data.lift=0;
            p->Control_count--;
        }
         if ( p->_data.auto_mode==0)
        {
            p->_data.turn=0;
        }
        p->Control_count++;
        Set_Acc_motor( p->_data.acc);
        Set_Lift_motor( p->_data.lift);
        Set_Turn_motor( -p->_data.turn);
        Set_AUTO( p->_data.auto_mode);
        sendonedata();
        Sleep(50);
    }
}
void APP_name::initdata()
{
   server_ip = "192.168.2.16";
   server_port = StateMachine_port;
   source_id = ID_Sensor_uwb;

   _handle.driver_name = "Car_control";
   _handle.driver_id = 1;
   
   //添加维护数据
   _handle.data_list.Add("acc", _data.acc);
   _handle.data_list.Add("turn", _data.turn);
   _handle.data_list.Add("lift", _data.lift);
    _handle.data_list.Add("auto_mode", _data.auto_mode);

    thread_base t(controlOnline,this);
    ERC212_initoutput();
}
void APP_name::datalist_up()
{
    _handle.data_list.Replace("acc", _data.acc);
    _handle.data_list.Replace("turn", _data.turn);
    _handle.data_list.Replace("lift", _data.lift);
    _handle.data_list.Replace("auto_mode", _data.auto_mode);
}
int APP_name::setDoubleValue(std::string type, double value)
{
     Control_count = 0;
  if(type=="acc")
  {
       _data.acc=value;
      return OK;
  }
  else if (type=="lift")
  {
      _data.lift=value;
      return OK;
  }
   else if (type=="turn")
  {
      if(_data.auto_mode==0)
      {
          _data.turn=0;
         return InManualState;
      }
      

       value= LimitingAmplitude(value,1.57);
      _data.turn=value;
      return OK;
  }
   else if (type=="auto_mode")
  {
       _data.auto_mode=value;
      return OK;
  }
  return ERR;
}
