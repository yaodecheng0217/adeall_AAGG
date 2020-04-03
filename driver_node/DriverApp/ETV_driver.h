/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 13:48:45
 * @LastEditTime: 2020-04-03 17:22:32
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#ifndef _DRIVER_APP_
#define _DRIVER_APP_

#include "drivernode.h"
#include <string.h>
#include "../ADS/ADS.h"
#define APP_name Car_control

class APP_name : public Driver_node
{
private:
    struct DATA
    {
        double acc = 0;
        double turn = 0;
        double lift = 0;
        double speed = 0;
        bool auto_mode = 0;

    } _data;
    double turnfeedback;
    ADS ads;
    void initdata();
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);
    int setDoubleValue(std::string type, double value);
    uint16_t Control_count = 10;
    static void *controlOnline(void *);
    void datalist_up();

    static void __stdcall CallbackV(AmsAddr *pAddr,
                                    AdsNotificationHeader *pNotification,
                                    unsigned long hUser);
    static void __stdcall CallbackT(AmsAddr *pAddr,
                                    AdsNotificationHeader *pNotification,
                                    unsigned long hUser);
    static void *pidControlthread(void *appprt);

public:
    APP_name(ProtocolAnalysis *msg);
    ~APP_name();
};
#endif