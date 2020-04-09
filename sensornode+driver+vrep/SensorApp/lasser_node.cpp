/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:45
 * @LastEditTime: 2020-04-07 10:00:53
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "lasser_node.h"

lasser_node::lasser_node(ProtocolAnalysis *msg, std::string name, uint8_t id) : Driver_node(msg)
{
   _handle.driver_id = id;
   _handle.driver_name = name;
}
lasser_node::~lasser_node()
{
}
void lasser_node::initdata()
{
   source_id = ID_Simulation;

   _handle.driver_name = "laser_sensor";
   _handle.driver_id = 1;
   //添加维护数据
   _handle.data_list.Add("TrayL_laser", _data.TrayL_laser);
   _handle.data_list.Add("TrayH_laser", _data.TrayH_laser);
   _handle.data_list.Add("high_laser", _data.high_laser);
   _handle.data_list.Add("forward_laser", _data.forward_laser);
   _handle.data_list.Add("side_laser", _data.side_laser);
}
void lasser_node::datalist_up()
{
   _handle.data_list.Replace("TrayL_laser", _data.TrayL_laser);
   _handle.data_list.Replace("TrayH_laser", _data.TrayH_laser);
   _handle.data_list.Replace("high_laser", _data.high_laser);
   _handle.data_list.Replace("forward_laser", _data.forward_laser);
   _handle.data_list.Replace("side_laser", _data.side_laser);
}
void lasser_node::updata(double TrayL_lasser,
                         double TrayH_lasser,
                         double high_lasser,
                         double forward_lasser,
                         double side_lasser)
{
   _data.forward_laser = forward_lasser;
   _data.TrayH_laser = TrayH_lasser;
   _data.TrayL_laser = TrayL_lasser;
   _data.side_laser = side_lasser;
   _data.high_laser = high_lasser;
}
int lasser_node::setDoubleValue(std::string type, double value)
{
   return 0;
}