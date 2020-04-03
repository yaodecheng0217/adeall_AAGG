/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-04-03 18:33:40
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "ETV_driver.h"
#include "udpinterface/thread_base.h"
#include "output.h"
#include "PID.h"
#include <fstream>
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
            p->_data.acc = 0;
            p->_data.lift = 0;
            p->Control_count--;
        }
        if (p->_data.auto_mode == 0)
        {
            p->_data.turn = 0;
        }
        p->Control_count++;
        //Set_Acc_motor(p->_data.acc);
        Set_Lift_motor(p->_data.lift);
        //Set_Turn_motor(-p->_data.turn);
        Set_AUTO(p->_data.auto_mode);
        sendonedata();
        Sleep(50);
    }
}
void APP_name::initdata()
{
    ads.ADS_init();
    ads.NotificationReq_MD(0, CallbackT, &turnfeedback);
    ads.NotificationReq_MD(1, CallbackV, &_data.speed);
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
    _handle.data_list.Add("speed", _data.speed);

    thread_base t(controlOnline, this);
    thread_base t2(pidControlthread, this);
    ERC212_initoutput();
}
void APP_name::datalist_up()
{
    _handle.data_list.Replace("acc", _data.acc);
    _handle.data_list.Replace("turn", _data.turn);
    _handle.data_list.Replace("lift", _data.lift);
    _handle.data_list.Replace("auto_mode", _data.auto_mode);
    _handle.data_list.Replace("speed", _data.speed);
}
int APP_name::setDoubleValue(std::string type, double value)
{
    Control_count = 0;
    if (type == "acc")
    {
        _data.acc = value;
        return OK;
    }
    else if (type == "lift")
    {
        _data.lift = value;
        return OK;
    }
    else if (type == "turn")
    {
        if (_data.auto_mode == 0)
        {
            _data.turn = 0;
            return InManualState;
        }

        value = LimitingAmplitude(value, 1.57);
        _data.turn = value;
        return OK;
    }
    else if (type == "auto_mode")
    {
        _data.auto_mode = value;
        return OK;
    }
    return ERR;
}
void __stdcall APP_name::CallbackT(AmsAddr *pAddr, AdsNotificationHeader *pNotification, ULONG hUser)
{
    double *data = (double *)hUser;
    *data = -*(float *)pNotification->data / 57.3;
    //printf("updatadata=%f  %f\n", -*(float *)pNotification->data, app->_data.turn * 57.3);
}
void __stdcall APP_name::CallbackV(AmsAddr *pAddr, AdsNotificationHeader *pNotification, ULONG hUser)
{
    double *data = (double *)hUser;
    *data = *(int *)pNotification->data / 100.0;
    // printf("updatadata=%d \n", -*(int *)pNotification->data);
}

void *APP_name::pidControlthread(void *appprt)
{
    static PID_LocTypeDef pid_info;
    pid_info.Kp = 0.011;
    double out = 0.01;
    double outV = 0;
    APP_name *app = (APP_name *)appprt;
    Set_Turn_motor(-out);

    while (1)
    {

        ifstream infile;
        infile.open("./conf.txt"); //将文件流对象与文件连接起来
        //assert(infile.is_open()); //若失败,则输出错误消息,并终止程序运行
        char c;
        infile >> noskipws;
        std::stringstream ss;
        while (!infile.eof())
        {
            infile >> c;
            ss << c;
        }

        infile.close(); //关闭文件输入流
        neb::CJsonObject ojson(ss.str());
        ojson.Get("Kp",pid_info.Kp);
         ojson.Get("Ki",pid_info.Ki);
          ojson.Get("Kd",pid_info.Kd);
        printf("%f %f %f ",pid_info.Kp,pid_info.Ki,pid_info.Kd);
        if (app->_data.auto_mode)
        {
            if (app->_data.acc == 0)
            {
                outV = 0;
                pid_info.LocSum = 0;
            }
            {
                if (app->_data.acc > 0)
                {
                    outV = 0.25 + PID_Loc(app->_data.acc, app->_data.speed, &pid_info);
                }

                if (app->_data.acc < 0)
                {
                    outV = -0.25 + PID_Loc(app->_data.acc, app->_data.speed, &pid_info);
                }
            }

            out = app->_data.turn;
        }
        else
        {
            app->_data.turn = app->turnfeedback;
            out = app->_data.turn;
            outV = 0;
            app->_data.acc = 0;
        }
        Set_Turn_motor(-out);
        Set_Acc_motor(outV);
        printf("%f  %f %f\n", app->_data.acc, app->_data.speed, outV);
        Sleep(50);
    }
}