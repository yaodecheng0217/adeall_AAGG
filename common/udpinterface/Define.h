/*
 * @Descripttion: 
 * @version: 
 * @Author: Yaodecheng
 * @Date: 2019-10-09 16:20:04
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2019-10-22 16:59:40
 */
#ifndef YADE_APP_DEFINES_H_
#define YADE_APP_DEFINES_H_


//这里举个例子
struct GetExportCargoNav_data
{
    double x;
    double y;
    double z;
    double yaw;
};

enum SOURCE_id
{
   AGV     = 0xD1,
   CAMMEAR = 0xD2,
   Server  = 0xD3,
};

enum DATA_type
{
Null_data             = 0,
GetExportCargoNav     = 1,
GetEmptyStockNav      = 2,
PutEmptyStockNav      = 3,
DetectWorkingStatus   = 4,
FinishOperate         = 5,
};




#endif