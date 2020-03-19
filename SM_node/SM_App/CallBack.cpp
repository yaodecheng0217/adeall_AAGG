/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:16:23
 * @LastEditors: Yaodecheng
 */
#include "app.h"
void APP::_Callback(ReturnFrameData in)
{
    switch (in.ins)
    {
    case INS_ACK:
    {
        _Callback_ACK(in);
    }
    break;
    case INS_GET:
    {
        _Callback_Get(in);
    }
    break;
    case INS_SET:
    {
        _Callback_Set(in);
    }
    break;
    case INS_HARBEAT:
    {
         _Callback_HEARBEAT(in);
    }
    break;
    default:
        break;
    }
}

void APP::_Callback_Get(ReturnFrameData in)
{
    //printf("in_cmd=%d\n",in.cmd_type);
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_GET_DATA:
    {
        
        _Send::TYPE_GET_DATA r;
        Decode_StructSerialize(&r, in._databuff);
        ACK_One_data(in.ip, in.port, r.type, r.seq);
    }
    break;
    default:
        break;
    }
}
void APP::_Callback_Set(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA:
    {
        _Send::TYPE_SET_DOUBLE_DATA r;
        Decode_StructSerialize(&r, in._databuff);
        int code = set_ControlValue(r.data.type, r.data.value);
        Set_ACK(in.ip, in.port, code, r.seq);
        //printf("driver set ack to contrl %d   %d \n",code,r.seq);
    }
    break;
    default:
        break;
    }
}
void APP::_Callback_ACK(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_ACK_SET:
    {
        _Send::TYPE_SET_ACK r;
        Decode_StructSerialize(&r, in._databuff);
        setCode(r.code, r.seq);
        //printf("driver set ack to SM %d   %d \n",r.code,r.seq);
    }
    break;
    default:
        break;
    }
}
void APP::_Callback_HEARBEAT(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_HEARBEAT_UWB_DATA:
    {
        _Send::TYPE_UWB_HEARBEAT_DATA xx;
        Decode_StructSerialize(&xx, in._databuff);
        SensorRsp(in.ip, in.port, xx.seq);
        AddNodeList(xx.handle, in.ip, in.port);  
        update(xx.handle, &xx.data);
    }
    break;
    case CMD_TYPE_LIST::CMD_HEARBEAT_ETV_DRIVER_DATA:
    {
        _Send::TYPE_ETV_DRIVER_HEARBEAT_DATA xx;
        Decode_StructSerialize(&xx, in._databuff);
        SensorRsp(in.ip, in.port, xx.seq);
        AddNodeList(xx.handle, in.ip, in.port);
        update(xx.handle, &xx.data);
    }
    break;
    default:
        break;
    }
}