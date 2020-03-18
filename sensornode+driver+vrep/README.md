<!--
 * @Author: Yaodecheng
 * @Date: 2020-03-10 21:14:45
 * @LastEditors: Yaodecheng
 -->
# Sensor node developing documents
1. 传感器应该有的属性
  * 名称 name 
  * 设备 driver id
  * 接口 api
  * 数据 data
  
## handle:
 * uint32 seq 对应一个标识符，随着消息被发布，它会自动增加
 * time stamp 存储与数据相关联的时间信息
 * string frame_id 字段存储与数据相关联的tf坐标系信息
  