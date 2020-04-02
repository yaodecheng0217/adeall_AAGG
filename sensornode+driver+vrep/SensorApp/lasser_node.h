/*
 * @Author: Yaodecheng
 * @Date: 2020-03-21 12:17:27
 * @LastEditTime: 2020-03-30 09:37:53
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */

#include "drivernode.h"
#include <string.h>
class lasser_node : public Driver_node
{
private:
    struct DATA
    {
        double TrayL_laser = 0;
        double TrayH_laser = 0;
        double high_laser = 0;
        double forward_laser = 0;
        double side_laser = 0;
    } _data;
    void initdata();
    void sendData(uint32_t seq, time_t timestamp);
    void sendHandle(uint32_t seq);
    int setDoubleValue(std::string type, double value);
    void datalist_up();

public:
    void updata(double TrayL_laser,
                double TrayH_laser,
                double high_laser,
                double forward_laser,
                double side_laser);
    lasser_node(ProtocolAnalysis *msg, std::string name, uint8_t id);
    ~lasser_node();
};
