/*
 * @Author: Yaodecheng
 * @Date: 2020-03-17 13:45:32
 * @LastEditTime: 2020-03-24 15:46:49
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "vrep_interface.h"
#include <stdio.h>
vrep_interface::vrep_interface(/* args */)
{
}

vrep_interface::~vrep_interface()
{
}

void vrep_interface::gethandle()
{
    simxGetObjectHandle(clientID, "forward_motor", &_handle.Forward, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "turn_motor", &_handle.Turnangle, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "action_motor", &_handle.Accelerator, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "lift_motor", &_handle.Lift, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "side_motor", &_handle.Side, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "Graph", &_handle.grap_handle, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "LaserPointer_sensor", &_handle.LaserHandle, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "LaserPointer_sensor#0", &_handle.LaserHandle0, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "LaserPointer_sensor#1", &_handle.LaserHandle1, simx_opmode_blocking);
    simxGetObjectHandle(clientID, "LaserPointer_sensor#2", &_handle.LaserHandle2, simx_opmode_blocking);
}
void vrep_interface::connect()
{
    clientID = simxStart("192.168.2.16", 19999, true, true, 1300, 5);
    if (clientID != -1)
    {
        printf("V-rep is connected.");
    }
    else
    {
        printf("V-rep can't be connected.");
    }
}
void vrep_interface::init()
{

    connect();
    if (clientID != -1)
    {
        gethandle();
        simxGetJointPosition(clientID, _handle.Side, NULL, simx_opmode_streaming);                              //侧移量，激光测距数据0
        simxReadProximitySensor(clientID, _handle.LaserHandle, NULL, NULL, NULL, NULL, simx_opmode_streaming);  //激光测距数据1
        simxReadProximitySensor(clientID, _handle.LaserHandle0, NULL, NULL, NULL, NULL, simx_opmode_streaming); //激光测距数据2
        simxReadProximitySensor(clientID, _handle.LaserHandle1, NULL, NULL, NULL, NULL, simx_opmode_streaming); //激光测距数据3
        simxReadProximitySensor(clientID, _handle.LaserHandle2, NULL, NULL, NULL, NULL, simx_opmode_streaming); //激光测距数据4
        simxGetObjectPosition(clientID, _handle.Turnangle, _handle.grap_handle, NULL, simx_opmode_streaming);   //uwb定位数据

        simxGetJointPosition(clientID, _handle.Turnangle, NULL, simx_opmode_streaming);                          //转角反馈
        simxGetJointForce(clientID, _handle.Accelerator, NULL, simx_opmode_streaming);                           //驱动轮力矩
        simxGetObjectVelocity(clientID, _handle.Accelerator, NULL, NULL, simx_opmode_streaming);                 //驱动轮速度
        simxGetJointPosition(clientID, _handle.Forward, NULL, simx_opmode_streaming);                            //前移位置
        simxGetObjectOrientation(clientID, _handle.Turnangle, _handle.grap_handle, NULL, simx_opmode_streaming); //四元数
        printf("sensor get handle.%d %d %d %d %d %d %d %d %d %d\n",
               _handle.Lift,
               _handle.Accelerator,
               _handle.Forward,
               _handle.Side,
               _handle.Turnangle,
               _handle.LaserHandle0,
               _handle.LaserHandle1,
               _handle.LaserHandle2,
               _handle.LaserHandle,
               _handle.grap_handle);
    }
}

void vrep_interface::PauseStep()
{
    simxPauseCommunication(clientID, true);
}
void vrep_interface::ContinueStep()
{
    simxPauseCommunication(clientID, false);
    simxSynchronousTrigger(clientID);
}
vrep_data vrep_interface::GetAllData()
{
    vrep_data d;
    float temp[3];
    simxGetJointPosition(clientID, _handle.Side, &d.side_lasser, simx_opmode_buffer);                   //侧移量，激光测距数据0
    simxReadProximitySensor(clientID, _handle.LaserHandle, NULL, temp, NULL, NULL, simx_opmode_buffer); //激光测距数据1
    d.TrayH_lasser = temp[2];
    simxReadProximitySensor(clientID, _handle.LaserHandle0, NULL, temp, NULL, NULL, simx_opmode_buffer); //激光测距数据2
    d.high_lasser = temp[2];
    simxReadProximitySensor(clientID, _handle.LaserHandle1, NULL, temp, NULL, NULL, simx_opmode_buffer); //激光测距数据3
    d.forward_lasser = temp[2];
    simxReadProximitySensor(clientID, _handle.LaserHandle2, NULL, temp, NULL, NULL, simx_opmode_buffer); //激光测距数据4
    d.TrayL_lasser = temp[2];
    simxGetObjectPosition(clientID, _handle.Turnangle, _handle.grap_handle, temp, simx_opmode_buffer); //uwb定位数据
    d.Uwb_x = temp[0] * 100;
    d.Uwb_y = temp[1] * 100;
    simxGetJointPosition(clientID, _handle.Turnangle, &d.TurnWheel_angle, simx_opmode_buffer); //转角反馈

    simxGetObjectOrientation(clientID, _handle.Turnangle, _handle.grap_handle, temp, simx_opmode_buffer); //四元数

    if (temp[2] < 0)
        temp[2] = 360 / 57.3 + temp[2];
    temp[2] = temp[2] + 180 / 57.3;
    if (temp[2] > 360 / 57.3)
        temp[2] = temp[2] - 360 / 57.3;

    d.Uwb_yaw = temp[2];
    //simxGetJointForce(clientID, _handle.Accelerator, NULL, simx_opmode_buffer);                           //驱动轮力矩
    simxGetObjectVelocity(clientID, _handle.Accelerator, NULL, &d.Accelerator_speed, simx_opmode_buffer); //驱动轮速度
    //simxGetJointPosition(clientID, _handle.Forward, NULL, simx_opmode_buffer);            //前移位置
    return d;
}

void vrep_interface::Set_Forward_motor(double value)
{
    simxSetJointTargetVelocity(clientID, _handle.Forward, value, simx_opmode_oneshot);
    simxSynchronousTrigger(clientID);
}
void vrep_interface::Set_Acc_motor(double value)
{
    simxSetJointTargetVelocity(clientID, _handle.Accelerator, value, simx_opmode_oneshot);
    simxSynchronousTrigger(clientID);
}
void vrep_interface::Set_Lift_motor(double value)
{
    simxSetJointTargetVelocity(clientID, _handle.Lift, value, simx_opmode_oneshot);
    simxSynchronousTrigger(clientID);
}
void vrep_interface::Set_Side_motor(double value)
{
    simxSetJointTargetVelocity(clientID, _handle.Side, value, simx_opmode_oneshot);
    simxSynchronousTrigger(clientID);
}
void vrep_interface::Set_Turn_motor(double value)
{
    simxSetJointTargetPosition(clientID, _handle.Turnangle, value, simx_opmode_oneshot);
    simxSynchronousTrigger(clientID);
}
