/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 13:45:32
 * @LastEditTime: 2020-03-17 15:31:50
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#ifndef _VREP_INTERFACE_H_
#define _VREP_INTERFACE_H_

extern "C"
{
#include "./remoteApi/extApi.h"
}

struct vrep_data
{
    float high_lasser = 0;
    float forward_lasser = 0;
    float side_lasser = 0;
    float TrayH_lasser = 0;
    float TrayL_lasser = 0;
    float Uwb_x = 0;
    float Uwb_y = 0;
    float Uwb_yaw = 0;
    float TurnWheel_angle = 0;
    float Accelerator_speed =0;
};

class vrep_interface
{
private:
    int clientID = -1;
    struct vrepHandle
    {
        simxInt Accelerator;
        simxInt Lift;
        simxInt Turnangle;
        simxInt Forward;
        simxInt Side;
        simxInt grap_handle;
        simxInt LaserHandle;
        simxInt LaserHandle0;
        simxInt LaserHandle1;
        simxInt LaserHandle2;
    } _handle;
    static void *threadfunction(void *prt);
    
    void gethandle();
    void connect();

public:
    void init();
    vrep_interface();
    ~vrep_interface();
    void PauseStep();
    void ContinueStep();

    vrep_data GetAllData();

    void Set_Forward_motor(double value);
    void Set_Acc_motor(double value);
    void Set_Lift_motor(double value);
    void Set_Side_motor(double value);
    void Set_Turn_motor(double value);
    //void Set_Brake(double value);
};

#endif
