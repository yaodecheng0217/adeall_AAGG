/*
 * @Descripttion: Adeall udp通讯协议解析
 * @version: V1.0
 * @Author: Yaodecheng
 * @Date: 2019-10-19 10:18:47
 * @LastEditors: Yaodecheng
 * @LastEditTime: 2020-03-20 17:28:30
 */

#ifndef _PREOTOCOLANALYSIS_H_
#define _PREOTOCOLANALYSIS_H_

#include "UdpMessage.h"

//序列化
#include <cereal/archives/binary.hpp>
#include "cereal/types/string.hpp" //一定要包含此文件，否则无法将std::string序列化为二进制形式，请看：https://github.com/USCiLab/cereal/issues/58

namespace msgpa
{
struct FrameDataStruct
{
    uint8_t source_id = 0;
    uint8_t cmd_id[2] = {0, 0};
    int8_t ins = -1;
    uint8_t cmd_type = 0;
    std::vector<uint8_t> _databuff;
};
struct ReturnFrameData
{
    char *ip;
    int port;
    uint8_t source_id = 0;
    uint8_t cmd_id[2] = {0, 0};
    int8_t ins = -1;
    uint8_t cmd_type = 0;
    std::vector<uint8_t> _databuff;
};
//不包含可变长度序列
template <typename T>
void Add_T_2_sendData(T in, FrameDataStruct *out)
{
    /*if (typeid(T) == typeid(std::string))
    {
        std::string data = (std::string)in;
        int size = data.size();
        out->_databuff.resize(size);
        for (size_t i = 0; i < size; i++)
        {
            out->_databuff[i] = data[i];
        }
        return;
    }*/
    out->_databuff.resize(sizeof(in));
    memcpy(&out->_databuff[0], &in, sizeof(in));
}
void Add_string_sendData(std::string data, FrameDataStruct *out);
//包含strin类型
template <typename T>
void Add_StructSerialize_sendData(T in, FrameDataStruct *out)
{
    std::stringstream ss;
    cereal::BinaryOutputArchive oarchive(ss);
    oarchive(in);

    std::string data = ss.str();
    int size = data.size();
    out->_databuff.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        out->_databuff[i] = data[i];
    }
}
//反序列化
template <typename T>
void Decode_StructSerialize(T *out, const std::vector<uint8_t> _databuff)
{
    std::stringstream ss;
    for (uint8_t x : _databuff)
        ss << x;
    cereal::BinaryInputArchive iarchive(ss);
    iarchive(*out);
}
template <typename T>
void Decode_Struct_No_Serialize(T *out, const std::vector<uint8_t> _databuff)
{
    *out = *(T *)&_databuff[0];
}
class ProtocolAnalysis : protected UdpMessage
{
    typedef void (*OutputDataFun)(ReturnFrameData data);

private:
    OutputDataFun _outputfun;
    void CallBackFuntion(std::vector<uint8_t>, void *);

public:
    ProtocolAnalysis(OutputDataFun);
    ~ProtocolAnalysis();
    int init(const int port);
    int sendData(const char *ip, int prot, FrameDataStruct sdata);

    template <typename T>
    int sendData(const char *tgr_ip,
                 const int tgr_prot,
                 uint8_t source_id,
                 int8_t ins,
                 uint8_t cmd_type,
                 T data)
    {
        FrameDataStruct Xdata;
        Xdata.source_id = source_id;
        Xdata.cmd_type = cmd_type;
        Xdata.ins = ins;
        Add_StructSerialize_sendData(data, &Xdata);
        return sendData(tgr_ip, tgr_prot, Xdata);
    }

    template <typename T>
    int sendData_N(const char *tgr_ip,
                   const int tgr_prot,
                   uint8_t source_id,
                   int8_t ins,
                   uint8_t cmd_type,
                   T data)
    {
        FrameDataStruct Xdata;
        Xdata.source_id = source_id;
        Xdata.cmd_type = cmd_type;
        Xdata.ins = ins;
        Add_T_2_sendData(data, &Xdata);
        return sendData(tgr_ip, tgr_prot, Xdata);
    }
};
} // namespace msgpa
#endif