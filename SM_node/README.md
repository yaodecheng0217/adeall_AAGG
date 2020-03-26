<!--
 * @Author: Yaodecheng
 * @Date: 2020-03-10 21:14:45
 * @LastEditors: Yaodecheng
 -->
# 《StateMachine 状态执管理器》
# 1.简介
>StateMachine是抽象底层与控制层之间的桥梁，主要负责监视所以在线传感器信息和驱动器信息，当上层应用对其进行访问的时候首先查询其对应节点信息，返回相应的查询结果，当上层应用发送执行指令时,将上级指令向下传递给驱动器，并反馈执行情况。
---
# 2.详细设计
##2.1 数据头
数据头用于设备向StateMachine上线注册，所有受管理的设备应该先发送handle到StateMachine，设备收到StateMachine的应答持续更新数据，无需再发送handle信息。如果设备掉线，StateMachine将自动删除其注册信息，设备需要重新发送handle进行注册。
handle的定义如下
```c++
struct DRIVER_HANDLE
{
    std::string driver_name;//设备名称
    std::string d
    uint32_t driver_id;//id标识了所代表的设备，id是唯一的
    uint32_t driver_type;//代表了该设备更新维护的数据类型
};
```
## 2.2设备id
设备id是设备唯一标识，通过id可以得知所有该设备信息，并且控制设备也是通过id进行
id|标识符|描述
--:|:--:|:--
1|id_Accelerator|主动轮油门
2|id_Brake|刹车量
3|id_TurnAngle|主动轮转向角
4|id_Lift|举升电机
5|id_Side|侧移电机
6|id_MoveForward|前移电机
7|id_Tilt|倾斜电机
8|id_LED_Green|绿色led
9|id_LED_Red|红色led
10|id_LED_Yellow|黄色led
11|id_Paking|驻车系统
12|id_AUTO|手自动切换器
13|id_Location|定位系统
14|id_laser_high|货叉高度
15|id_laser_forward|前移距离
16|id_laser_TrayH|货叉托盘距离
17|id_laser_TrayL|底部托盘距离
18|id_laser_side|侧移激光测距
19|id_TurnWheel|转向反馈
20|id_AccSpeed|主动轮速度反馈
21|id_SICK_rearC|后中激光避障
22|id_SICK_rearL|后左激光避障
23|id_SICK_rearR|后右激光避障
24|id_SICK_Ahead|前置激光避障
25|id_ForkSeparation|货叉分离
26|id_ForkHorizontal|货叉水平位
27|id_ForkTeeth|叉齿防撞
28|id_Forkweight|货叉称重

## 2.3数据类型
StateMachine管理的所有数据数据类型在**DIRVER_TYPE_LIST**进行定义，数据类型定义用于区分不同的数据，从而进行存储和读取
传感器|数据类型|描述
:--:|:--:|:--
uwb|LOCATION_DATA|包含坐标信息
相机|CCAMEAR_DATA|--待定义--
驱动|CAR_DRIVER_DATA|包含车辆执行器所有控制数据，目前以ETV车型为参考，控制量较为全面
SICK激光|SICK_DATA|包含两个SICK状态数据，以及两个输出数据
浮点数|DOUBLE_DATA|包含一个double类型数据，此类型可以用于发布转角仪或者激光测距等只有一个数据输出的数据
开关量|BOOL_DATA|包含一个bool类型数据，像车辆灯光风扇
整形|INT32_DATA|包含一个int32_t类型数据



## 开发任务

  