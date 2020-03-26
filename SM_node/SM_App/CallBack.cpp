/*
 * @Author: Yaodecheng
 * @Date: 2020-03-16 09:16:23
 * @LastEditors: Yaodecheng
 */
#include "app.h"
#include "CJson/cJSON.h"
#include "CJson/CJsonObject.hpp"

void APP::_Callback(ReturnFrameData in)
{
   
    //return ;
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
        
        TYPE_GET_DATA r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        ACK_One_data(in.ip, in.port, r.type, r.seq);
    }
    break;
    default:
        break;
    }
}
void APP::_Callback_Set(ReturnFrameData in)
{
    //printf("in_cmd=%d\n",in.cmd_type);
    switch (in.cmd_type)
    {
    case CMD_TYPE_LIST::CMD_SET_DOUBLE_DATA:
    {
        TYPE_SET_DOUBLE_DATA r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        int code = set_ControlValue(r.type, r.value);
        Set_ACK(in.ip, in.port, code, r.seq);
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
        TYPE_ACK_CODE r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        //printf("driver set ack to SM %d   %d \n",r.code,r.seq);
        recvAckCode(r.code, r.seq);
        
    }
    break;
     case CMD_TYPE_LIST::CMD_ACK_CODE:
    {
        TYPE_ACK_CODE r;
        Decode_Struct_No_Serialize(&r, in._databuff);
        recvAckCode(r.code, r.seq);
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
    case CMD_TYPE_LIST::CMD_HEARBEAT_HANDLE:
    {
        std::stringstream ss;
        for (uint8_t x:in._databuff)
        {
           ss<<x;
        }
        neb::CJsonObject oJson(ss.str().c_str());
        printf("\n%s\n",oJson.ToFormattedString().c_str());
        DRIVER_HANDLE handle;
        TYPE_handle_string s;
        oJson.Get("driver_name",handle.driver_name);
        oJson.Get("driver_id",handle.driver_id);
        oJson.Get("data_type",handle.data_type);
        oJson.Get("data_size",handle.data_size);
        oJson.Get("data_name",handle.data_name);
        uint32_t seq;
        oJson.Get("seq",seq);
        //printf("%s  %d  %d  %d\n",handle.driver_name.c_str(),handle.datatype,handle.driver_id,seq); 
        SensorRsp(in.ip, in.port,seq,OK);
        AddNodeList(handle, in.ip, in.port);
        //printf(".");
    }
    break;
    case CMD_TYPE_LIST::CMD_UPDATE_DATA:
    {
        std::stringstream ss;
        for (uint8_t x:in._databuff)
        {
           ss<<x;
        }
        
        neb::CJsonObject oJson(ss.str().c_str());
        //printf("\n%s\n",oJson.ToFormattedString().c_str());
        uint32_t seq;
        oJson.Get("seq",seq);
        if(UpdateDataDetail(oJson["update"]))
        {
            SensorRsp(in.ip, in.port, seq,OK);
        }
        else
        {
            SensorRsp(in.ip, in.port, seq,ERR);
        }
    }
    break;
    default:
        break;
    }
}