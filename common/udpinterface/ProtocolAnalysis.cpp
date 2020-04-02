/*
 * @Descripttion: 
 * @version: 
 * @Author: Yaodecheng
 * @Date: 2019-10-19 10:26:48
 * @LastEditors: Yaodecheng
 * @LastEditTime: 2020-03-23 18:19:41
 */

#include "ProtocolAnalysis.h"
namespace msgpa
{
	void Add_string_sendData(std::string data, FrameDataStruct *out)
{
    int size = data.size();
    out->_databuff.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        out->_databuff[i] = data[i];
    }
}
ProtocolAnalysis::ProtocolAnalysis(OutputDataFun f)
    : _outputfun(f)
{
}

ProtocolAnalysis::~ProtocolAnalysis()
{
}

int ProtocolAnalysis::init(const int port)
{
    return UdpMessage::init(port, this);
}
struct MyStruct
{
    uint32_t x;
    float y;
};

int ProtocolAnalysis::sendData(const char *ip, int prot, FrameDataStruct sdata)
{
    uint16_t Version = 1;
    uint32_t dataLength = sdata._databuff.size();
    std::vector<uint8_t> sendbuff;

    sendbuff.resize(dataLength + 21);

    sendbuff[0] = 0x7f;
    sendbuff[1] = 0xf7;
    sendbuff[2] = (uint8_t)Version;
    sendbuff[3] = (uint8_t)(Version >> 8);

    sendbuff[4] = sdata.ins;
    sendbuff[5] = 0;
    sendbuff[6] = 0;

    sendbuff[7] = sdata.source_id;

    sendbuff[8] = sdata.cmd_id[0];
    sendbuff[9] = sdata.cmd_id[1];

    sendbuff[10] = sdata.cmd_type;

    sendbuff[11] = (uint8_t)(dataLength);
    sendbuff[12] = (uint8_t)(dataLength >> 8);
    sendbuff[13] = (uint8_t)(dataLength >> 16);
    sendbuff[14] = (uint8_t)(dataLength >> 24);

    if (dataLength > 0)
        memcpy(&sendbuff[15], &sdata._databuff[0], dataLength);

    sendbuff[16 + dataLength] = 0;
    sendbuff[17 + dataLength] = 0;
    sendbuff[18 + dataLength] = 0;

    //计算校验和除去帧头帧尾,校验和放在三个校验位最后一位
    /*
    发送方：对要数据累加，得到一个数据和，对和求反，即得到我们的校验值。然后把要发的数据和这个校验值一起发送给接收方。
    接收方：对接收的数据（包括校验和）进行累加，然后加1，如果得到0，那么说明数据没有出现传输错误。
    （注意，此处发送方和接收方用于保存累加结果的类型一定要一致，否则加1就无法实现溢出从而无法得到0，校验就会无效）
    */
    for (size_t i = 2; i < 18 + dataLength; i++)
    {
        sendbuff[18 + dataLength] = sendbuff[18 + dataLength] + sendbuff[i];
    }
    sendbuff[18 + dataLength] = ~sendbuff[18 + dataLength];

    sendbuff[19 + dataLength] = 0xf7;
    sendbuff[20 + dataLength] = 0x7f;

    return messagsend(ip, prot, (char *)&sendbuff[0], dataLength + 21);
}

void ProtocolAnalysis::CallBackFuntion(std::vector<uint8_t> databuffer, void *ptr)
{

    uint32_t bufferLength = databuffer.size();
   /* for (size_t i = 0; i < bufferLength; i++)
    {
        printf("%X ", databuffer[i]);
    }
    printf("\n");*/
    //printf("-->%d\n",databuffer.size());

    if (databuffer[0] == 0x7f && databuffer[1] == 0xf7 && databuffer[bufferLength - 2] == 0xf7 && databuffer[bufferLength - 1] == 0x7f)
    {

        uint16_t Version;
        memcpy(&Version, &databuffer[2], 2);

        switch (Version)
        {
        case 1:
        {
            //printf("version= %d\n",Version);
            //检查校验和
            uint8_t ret = 0;
            for (size_t i = 2; i < bufferLength - 2; i++)
            {
                ret += databuffer[i];
                //printf("%d ",databuffer[i]);
            }
            //printf("\n");
            ret += 1;
            if (ret != 0)
            {
                printf("Checksum error = %d\n", ret);
                return;
            }

            ReturnFrameData out;
            out.ins = databuffer[4];
            out.source_id = databuffer[7];
            out.cmd_id[0] = databuffer[8];
            out.cmd_id[1] = databuffer[9];
            out.cmd_type = databuffer[10];

            uint32_t datalength;
            memcpy(&datalength, &databuffer[11], 4);
            out._databuff.resize(datalength);
            if (datalength > 0)
                memcpy(&out._databuff[0], &databuffer[15], datalength);
            out.port = ntohs(addr_client.sin_port);
            out.ip = inet_ntoa(addr_client.sin_addr);
            //printf("%d\n",uu++);
            _outputfun(out);
        }
        break;

        default:
            printf("ProtocolAnalysis Verstion ERROR !\n");
            break;
        }
    }
    else
     printf("ProtocolAnalysis Verstion ERROR !\n");
    return;
}
} // namespace msgpa