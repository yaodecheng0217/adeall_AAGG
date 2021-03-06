/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:16:23
 * @LastEditors: Yaodecheng
 */
#include "app.h"
void APP::_Callback(ReturnFrameData in)
{static int x=0;
    //printf("*");
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
    switch (in.cmd_type)
    {
    default:
        break;
    }
}
void APP::_Callback_Set(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
        break;
    default:
        break;
    }
}
void APP::_Callback_ACK(ReturnFrameData in)
{
    switch (in.cmd_type)
    {
    case CMD_ACK_LOCATION_DATA:
    {
       // printf("ack loc ");
        TYPE_ACK_LOCATION_DATA r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        reaction_ACK(&r,r.seq);
    }
    break;
    case CMD_ACK_ONE_DATA:
    {
        //printf("ack double ");
         TYPE_ACK_ONE_DATA r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        reaction_ACK(&r,r.seq);
    }
    break;
    case CMD_ACK_SET:
    {
       TYPE_ACK_CODE r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        setCode(r.code,r.seq);
    }
    break;
    case CMD_ACK_CODE:
    {
        //printf("ack set ");
       TYPE_ACK_CODE r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        setCode(r.code,r.seq);
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
        break;
    default:
        break;
    }
}