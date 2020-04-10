/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-04-10 18:35:36
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "readconfig/readjson.h"
#include "time_util.h"
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
        if (p->Control_count > 10)
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
        Sleep(50);
    }
}
void APP_name::initdata()
{
    ads.ADS_init();
    ads.NotificationReq_MD(0, CallbackT, &turnfeedback);
    ads.NotificationReq_MD(1, CallbackV, &_data.speed);

    Readconfig config("Initconfig.json");
    config.setitem("driver");
    config.GetValue("low_acc", low_acc);
    config.GetValue("high_acc", high_acc);

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
    thread_base t3(timer50msthread, this);
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
        printf("set acc %f\n",_data.acc);
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
    *data = *(int *)pNotification->data / 500.0;
    // printf("updatadata=%d \n", -*(int *)pNotification->data);
}

void *APP_name::pidControlthread(void *appprt)
{
    static PID_IncTypeDef aac_pid;
    static PID_IncTypeDef turn_pid;
    aac_pid.Kp = 0.011;
    turn_pid.Kp = 0.5;
    //double out = 0.01;
    double outV = 0;
    double outT = 0;
    double down_k = 5;
    double test = 0.1;
    APP_name *app = (APP_name *)appprt;
    ReadSeting set("driver.json");
    std::ofstream logout("log.csv");
    while (1)
    {

        set.reload();
        set.GetValue("kp", aac_pid.Kp);
        set.GetValue("ki", aac_pid.Ki);
        set.GetValue("kd", aac_pid.Kd);
        set.GetValue("Tkp", turn_pid.Kp);
        set.GetValue("Tki", turn_pid.Ki);
        set.GetValue("Tkd", turn_pid.Kd);
        set.GetValue("down_k", down_k);
        set.GetValue("test", test);
        //printf("%f %f %f ", aac_pid.Kp, aac_pid.Ki, aac_pid.Kd);
        if (app->_data.auto_mode)
        {
            if (app->_data.acc == 0)
            {
                outV = 0;
                outT = app->turnfeedback;
                //pid_info.LocSum = 0;
            }
            {

                double dta = PID_Inc(app->_data.acc, app->_data.speed, &aac_pid);

                if (dta * app->_data.acc > 0)
                    outV = outV + dta;
                else
                {
                    outV = outV + dta * ((dta * dta) + 1) * down_k;
                }
                if (app->_data.acc > 0)
                {
                    if (outV < 0)
                        outV = 0;
                }
                else if (app->_data.acc < 0)
                {
                    if (outV > 0)
                        outV = 0;
                }
            }
        }
        else
        {
            app->_data.turn = app->turnfeedback;
            outV = 0;
            app->_data.acc = 0;
        }

        outT = outT + PID_Inc(app->_data.turn, outT, &turn_pid);

        Set_Turn_motor(outT);
        if (app->_data.acc > 0)
            Set_Acc_motor(outV + app->high_acc);
        else if (app->_data.acc < 0)
            Set_Acc_motor(outV + app->low_acc);
        else
            Set_Acc_motor(0);

        Set_Lift_motor(app->_data.lift);
        Set_AUTO(app->_data.auto_mode);
        sendonedata();
        double time = GetCurrentTime();
        logout << time << ","
               << app->_data.acc << ","
               << outV << ","
               << app->_data.speed << ","
               << app->_data.turn << ","
               << outT << ","
               << app->_data.auto_mode << std::endl;
        printf("%f  %f %f %d\n", app->_data.acc, app->_data.speed, outV, app->_data.auto_mode);
        app->timer50L.lock();
    }
}
void *APP_name::timer50msthread(void *appprt)
{
    APP_name *app = (APP_name *)appprt;
    while (1)
    {
        Sleep(50);
        app->timer50L.unlock();
    }
}