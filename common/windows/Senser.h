#pragma once
#include"ComCTR.h"

struct IMUdata {
	double yaw;         //偏航
	double pitch;      //俯仰
	double roll;       //横滚
	int rate;        //解算速率

	int Accx;       //x加速度
	int Accy;       //y
	int Accz;       //z
	int Gyrox;      //陀螺仪x
	int Gyroy;      //y
	int Gyroz;       //z
	int Mx;          //磁力计
	int My;           //y
	int Mz;           //z

	int  height, temperature, prssure;
};
struct LocalSenerData
{
	int Tagid, Posx, Posy, Posz;//标签id    坐标  x，y，z
	long long int TimeStam;     //时间锉
};

struct LocalSenerNode {
	int LocalID;           //基站id
	int range;             //距离
	long long int time;    //时间锉
	LocalSenerNode* next;
};
class LocalSenerLinklist
{
private:
	LocalSenerNode* Head;
	int Length = 0;
public:
	LocalSenerLinklist();
	~LocalSenerLinklist();
	void Insert(int id);
	void Update(int id, int range, long long int time);
	void Delet(int id);
	int isEmpty();
	int find(int id);
	LocalSenerNode* GetData(int id);
	int ListLength();
	void Display();
};

class UWBsenser
{
public:
	UWBsenser();
	~UWBsenser();

	
	ComCTR com1, com2;//一个uwb标签需要两个串口

	/*imu数据结构存放imu数据*/
	IMUdata imu_data;
	/*uwb定位数据结构存放基站数据*/
	LocalSenerData localsenser_data;
	/*接受到基站的信息，因为存在个数的无限性，存放在链表中*/
	LocalSenerLinklist localsenserlist;

	UCHAR star(const char* szStr1, const char* szStr2);//启动uwb定位解算，传入com口信息
	void updatacon();
	/*定位标签是否在线*/
	bool uwbonline = false;
};





