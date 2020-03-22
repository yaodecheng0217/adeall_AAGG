#include "Senser.h"
#include <time.h>

struct IMUdata IMU_data, IMU_data2;
struct LocalSenerData LocalSenerData1, LocalSenerData2;


LocalSenerLinklist LocalSenerLinklist1, LocalSenerLinklist2;

LocalSenerLinklist::LocalSenerLinklist() {
	Head = new LocalSenerNode;
	Head->next = NULL;;
}
int LocalSenerLinklist::isEmpty()
{
	if (Head->next == NULL)
		return 1;
	else
		return 0;
}
int LocalSenerLinklist::find(int id)
{
	if (isEmpty())
		return 0;
	LocalSenerNode *p;
	p = Head->next;
	while (1)
	{
		if (p->LocalID == id)
		{

			return 1;
		}
		if (p->next == NULL)
			return 0;
		else
			p = p->next;
	}

}
void LocalSenerLinklist::Insert(int id) {


	LocalSenerNode * p;
	LocalSenerNode * temp;
	p = Head;
	temp = new LocalSenerNode;
	temp->LocalID = id;
	temp->next = NULL;

	if (find(id))
	{
		std::cout << "id is being!" << std::endl;
		delete (temp);
		return;
	}
	while (p->next != NULL)
	{
		p = p->next;
	}
	p->next = temp;
	Length++;
	//logy << "##"<<temp->LocalID << "ID is insert" << std::endl;

}
void LocalSenerLinklist::Update(int id, int range, long long int time)
{
	if (isEmpty())
		return;
	LocalSenerNode * p;
	p = Head->next;
	while (1)
	{
		if (p->LocalID == id)
		{
			p->range = range;
			p->time = time;
			return;
		}
		if (p->next == NULL)
			return;
		else
			p = p->next;
	}
}
void LocalSenerLinklist::Delet(int id) {
	if (isEmpty())
		return;
	LocalSenerNode *p, *temp;
	p = Head->next;
	temp = Head;
	while (true)
	{
		if (p->LocalID == id)
		{
			temp->next = p->next;
			delete(p);
			Length--;
			return;
		}

		if (p->next == NULL)
			return;
		else
		{
			p = p->next;
			temp = temp->next;
		}
		
	}
	std::cout << "delet error!!" << std::endl;
}
LocalSenerNode *  LocalSenerLinklist::GetData(int id)
{
	if (isEmpty())
		return NULL;
	LocalSenerNode *p;
	p = Head->next;
	while (true)
	{
		if (p->LocalID == id)
		{
			return p;
		}
		if (p->next == 0)
			return NULL;
		else
		{
			p = p->next;
		}
	}
	return NULL;
}
void LocalSenerLinklist::Display() {

	if (isEmpty())
	{
		std::cout << "is empty!" << std::endl;
		return;
	}
	LocalSenerNode *p;
	p = Head->next;
	std::cout << "List length=" <<Length<< std::endl;
	while (true)
	{
		std::cout << " **ID:" << p->LocalID << " Range=" << p->range << " time=" << p->time ;// std::endl;
		if (p->next == NULL)
			break;
		else
		{
			p = p->next;
		}
	}
	std::cout << std::endl;

}
int LocalSenerLinklist::ListLength() {
	if (isEmpty())
		return 0;
	LocalSenerNode *p;
	int N = 1;
	p = Head->next;
	while (true)
	{
		if (p->next == NULL)
			return N;
		else
		{
			p = p->next;
			N++;
		}
	}
}
LocalSenerLinklist::~LocalSenerLinklist()
{
}



void Decode0xA1(int data[])
{
	int yaw, pitch, roll, rate, temp, height, temperature, prssure;
	temp = data[1];
	temp = (temp << 8 | data[2]);
	yaw = temp;
	temp = data[3];
	temp = (temp << 8 | data[4]);
	pitch = temp;
	temp = data[5];
	temp = (temp << 8 | data[6]);
	roll = temp;
	temp = data[7];
	temp = (temp << 8 | data[8]);
	height = temp;
	temp = data[9];
	temp = (temp << 8 | data[10]);
	temperature = temp;
	temp = data[11];
	temp = (temp << 8 | data[12]);
	prssure = temp;
	temp = data[13];
	temp = (temp << 8 | data[14]);
	rate = temp;

	//std::cout << "  yaw="<<yaw<<"  pitch="<<pitch<<"  roll="<<roll<<"  rate="<<rate<<std::endl;
	// cout << "height=" << height << "temperature=" << temperature << "pressuure=" << prssure << "time="<< endl;
	if (yaw > 32768)
		yaw = 32768 - yaw;
	if (pitch > 32768)
		pitch = 32768 - pitch;
	if (roll > 32768)
		roll = 32768 - roll;

	IMU_data2.yaw = yaw / 10.0;
	IMU_data2.pitch = pitch / 10.0;
	IMU_data2.roll = roll / 10.0;
	IMU_data2.rate = rate;

	IMU_data2.height = height;
	IMU_data2.temperature = temperature;
	IMU_data2.prssure = prssure;

}
struct UWBD
{
	int ID;
	int rang;
};
void LocalSenerDealdata(string str,int L, UWBsenser* P)
{
;
	if (L > 2000)
		return;
	unsigned char data[2000] ;//转换类型
	
	for (size_t i = 0; i < 2000; i++)
	{
		data[i] = 0;
	}
	for (size_t i = 0; i < L; i++)
	{
		data[i] = str[i];
		//printf("%X ", data[i]);
	}
	//printf("\n");
	/*for (size_t i = 0; i < L; i++)
	{
		printf("%d ", data[i]);
	}
	printf("\n");
	
	uint8_t tsum = 0;
	for (int i = 0; i < 3; i++)
	{
		tsum = tsum + data[i];
		cout << (int)data[i] << "_";
	}
	tsum = tsum & 0xff;
	printf("   ==%d\n", tsum);*/


		//cout << "star is ok!" << (int)data[2] << endl;
		/*如果帧类型是0x2a或者0x2d，则是定位数据*/
		if (data[0] == 0x2a || data[0] == 0x2d)
		{
			
			int sum = 0;
			for (int i = 0; i < L-2-2; i++)
			{
				sum = sum + data[i];
				
				//cout << (int)data[i] << "_";
			}
			
			sum = sum & 0xff;
			//cout <<sum<<"-"<<(int)data[L-2]<<endl;
			if (sum != data[L - 1-2])//sumCheck
			{
				cout << "sumCheck is error1!" << endl;
				/*for (size_t i = 0; i < L; i++)
				{
					cout << (int)data[i] << "_";
					if(i%10==0)
						cout << endl;
				}
				cout << endl;*/
			}
			else
			{
				//cout << "sumCheck is ok 1!" << endl;
				switch (data[0])//帧类型
				{
				case 0x2a:
				{
					int Tagid, Posx, Posy, Posz;
					long long int TimeStam, temp;
					
					temp = data[1 + 1];
					temp = (data[2 + 1] << 8) | temp;
					Tagid = temp;

					temp = data[6 + 1];
					temp = (temp << 8) | data[5 + 1];
					temp = (temp << 8) | data[4 + 1];
					temp = (temp << 8) | data[3 + 1];
					Posx = temp;

					temp = data[10 + 1];
					temp = (temp << 8) | data[9 + 1];
					temp = (temp << 8) | data[8 + 1];
					temp = (temp << 8) | data[7 + 1];
					Posy = temp;

					temp = data[14 + 1];
					temp = (temp << 8) | data[13 + 1];
					temp = (temp << 8) | data[12 + 1];
					temp = (temp << 8) | data[11 + 1];
					Posz = temp;

					temp = data[22 + 1];
					temp = (temp << 8) | data[21 + 1];
					temp = (temp << 8) | data[20 + 1];
					temp = (temp << 8) | data[19 + 1];
					temp = (temp << 8) | data[18 + 1];
					temp = (temp << 8) | data[17 + 1];
					temp = (temp << 8) | data[16 + 1];
					temp = (temp << 8) | data[15 + 1];
					TimeStam = temp;

				    //cout << "Tagid=" << Tagid << " Posx=" << Posx << " Posy=" << Posy << " Posz=" << Posz << " TimeStam=" << TimeStam << std::endl;
					P->localsenser_data.Posx = Posx;
					P->localsenser_data.Posy = Posy;
					P->localsenser_data.Posz = Posz;
					P->localsenser_data.Tagid = Tagid;
					P->localsenser_data.TimeStam = TimeStam;
					
				}break;
				case 0x2d:
				{
					int N = 0;
					N = (data[1] - 21 - 3) / 3;
		
					int Tagid, DisInfos[50][3];
					long long int TimeStam;
					long long int temp;

					temp = data[2];
					temp = (temp << 8) | data[1];
					Tagid = temp;

					/*for (int i = L - 15; i < L-7; i++)
					{

						cout << (int)data[i] << "-";
					}
					cout << endl;*/
					for (int i = 0; i < N; i++) {

						temp = data[5 + i * 6];
						temp = (temp << 8) | data[4 + i * 6];
						DisInfos[i][0] = temp;//ID

						temp = data[7 + i * 6];
						temp = (temp << 6) | data[8 + i * 6];
						DisInfos[i][1] = temp;//距离

						temp = data[9 + i * 6];
						temp = (temp << 8) | data[8 + i * 6];
						DisInfos[i][2] = temp;//空

						//cout << "i=" <<i<<":"<< DisInfos[i][0] <<"    "<< DisInfos[i][1]<< endl;
						//fout << "i=" << i << endl;
							

					}
					
					temp = data[L - 8];
					temp = (temp << 8) | data[L - 9];
					temp = (temp << 8) | data[L - 10];
					temp = (temp << 8) | data[L - 11];
					temp = (temp << 8) | data[L - 12];
					temp = (temp << 8) | data[L - 13];
					temp = (temp << 8) | data[L - 14];
					temp = (temp << 8) | data[L - 15];


					TimeStam = temp;
					
					if (P->localsenserlist.find(DisInfos[0][0]) == 0)
					{
						P->localsenserlist.Insert(DisInfos[0][0]);
					}
					P->localsenserlist.Update(DisInfos[0][0], DisInfos[0][1], TimeStam);
					//P->localsenserlist.Display();
				}break;
				default:
					break;
				}

			}
		}
		if (data[1] == 0xA1 || data[1] == 0xA2)
		{
		
			int FrameL = data[2];
			int sum = 0;
			for (int i = 0; i < L - 2; i++)
			{
				sum = sum + data[i];
				//cout << (int)data[i] << "_";
			}
			//cout << FrameL<< endl;
			sum = sum & 0xff;
			//cout <<sum<<"-"<<(int)data[L-2]<<endl;
			if (sum != data[L - 2])//sumCheck
			{
				cout << "sumCheck is error2!" << endl;
			}
			else
			{
				//cout << "sumCheck is ok2!" << endl;
				/*开始解析*/
				switch (data[1])
				{
				case 0xa1:
				{
					int yaw, pitch, roll, rate, temp, height, temperature, prssure;
					temp = data[1 + 1];
					temp = (temp << 8 | data[2 + 1]);
					yaw = temp;
					temp = data[3 + 1];
					temp = (temp << 8 | data[4 + 1]);
					pitch = temp;
					temp = data[5 + 1];
					temp = (temp << 8 | data[6 + 1]);
					roll = temp;
					temp = data[7 + 1];
					temp = (temp << 8 | data[8 + 1]);
					height = temp;
					temp = data[9 + 1];
					temp = (temp << 8 | data[10 + 1]);
					temperature = temp;
					temp = data[11 + 1];
					temp = (temp << 8 | data[12 + 1]);
					prssure = temp;
					temp = data[13 + 1];
					temp = (temp << 8 | data[14 + 1]);
					rate = temp;

					//std::cout << "  yaw="<<yaw<<"  pitch="<<pitch<<"  roll="<<roll<<"  rate="<<rate<<std::endl;
					// cout << "height=" << height << "temperature=" << temperature << "pressuure=" << prssure << "time="<< endl;
					if (yaw > 32768)
						yaw = 32768 - yaw;
					if (pitch > 32768)
						pitch = 32768 - pitch;
					if (roll > 32768)
						roll = 32768 - roll;

					P->imu_data.yaw = yaw / 10.0;
					P->imu_data.pitch = pitch / 10.0;
					P->imu_data.roll = roll / 10.0;
					P->imu_data.rate = rate;

					P->imu_data.height = height;
					P->imu_data.temperature = temperature;
					P->imu_data.prssure = prssure;
					
				}break;
				case 0xA2:
				{
					int Accx, Accy, Accz, Gyrox, Gyroy, Gyroz, Mx, My, Mz,temp;

					temp = data[1 + 1];
					temp = (temp << 8 | data[2 + 1]);
					Accx = temp;

					temp = data[3 + 1];
					temp = (temp << 8 | data[4 + 1]);
					Accy = temp;


					temp = data[5 + 1];
					temp = (temp << 8 | data[6 + 1]);
					Accz = temp;

					temp = data[7 + 1];
					temp = (temp << 8 | data[8 + 1]);
					Gyrox = temp;


					temp = data[9 + 1];
					temp = (temp << 8 | data[10 + 1]);
					Gyroy = temp;


					temp = data[11 + 1];
					temp = (temp << 8 | data[12 + 1]);
					Gyroz = temp;

					temp = data[13 + 1];
					temp = (temp << 8 | data[14 + 1]);
					Mx = temp;

					temp = data[15 + 1];
					temp = (temp << 8 | data[16 + 1]);
					My = temp;

					temp = data[17 + 1];
					temp = (temp << 8 | data[18 + 1]);
					Mz = temp;


					//cout << "  Accx="<<Accx<<"  Accy="<<Accy<<"  Accz="<<Accz<<"  Gyrox="<<Gyrox<<"  Gyroy="<<Gyroy<<"  Gyroz="<<Gyroz<<"  Mx="<<My<<"  My="<<My<<"  Mz="<<Mz<<endl;
					if (Accx > 32768)
						Accx = 32768 - Accx;
					if (Accy > 32768)
						Accy = 32768 - Accy;
					if (Accz > 32768)
						Accz = 32768 - Accz;

					if (Gyrox > 32768)
						Gyrox = 32768 - Gyrox;
					if (Gyroy > 32768)
						Gyroy = 32768 - Gyroy;
					if (Gyroz > 32768)
						Gyroz = 32768 - Gyroz;

					if (Mx > 32768)
						Mx = 32768 - Mx;
					if (My > 32768)
						My = 32768 - My;
					if (Mz > 32768)
						Mz = 32768 - Mz;
					//cout << "  Accx=" << Accx << endl;
					P->imu_data.Accx = Accx;
					P->imu_data.Accy = Accy;
					P->imu_data.Accz = Accz;
					P->imu_data.Gyrox = Gyrox;
					P->imu_data.Gyroy = Gyroy;
					P->imu_data.Gyroz = Gyroz;
					P->imu_data.Mx = Mx;
					P->imu_data.My = My;
					P->imu_data.Mz = Mz;
					
				}break;
				default:
					break;
				}

			}
		}
}

DWORD WINAPI  UWBsenser::UWBthread1(void* args)
{
	UWBsenser* P = (UWBsenser*)args;
	while (1)
	{
		if (P->com1.Recv_length>0)
		{
			int L = P->com1.Recv_length;
			string str = P->com1.read();
			LocalSenerDealdata(str, L, P);
		}
		Sleep(10);
	}
}
DWORD WINAPI  UWBsenser::UWBthread2(void* args)
{
	UWBsenser* P = (UWBsenser*)args;
	while (1)
	{
		if (P->com2.Recv_length > 0)
		{
			int L = P->com2.Recv_length;
			string str = P->com2.read();
			LocalSenerDealdata(str, L, P);
		}
		Sleep(10);
	}
}
DWORD WINAPI  UWBsenser::onlineChick(void* args)
{
	UWBsenser* P = (UWBsenser*)args;
	long long int temperature = 0;
	while (true)
	{
		Sleep(500);
		//cout << P->localsenser_data.TimeStam << endl;
		if (P->localsenser_data.TimeStam == temperature)
		{
			P->uwbonline = false;
		}
		else
		{
			temperature = P->localsenser_data.TimeStam;
			P->uwbonline = true;
		}
	}

}



UWBsenser::UWBsenser()
{
	
}

UWBsenser::~UWBsenser()
{
}

typedef bool (*pFun)();


DWORD WINAPI  UWBthread(void* args)
{
	pFun P = (pFun)args;
	while (1)
	{
		P();
		Sleep(10);
	}
}

void UWBsenser::updatacon()
{
	if (com1.Recv_length > 0)
	{
		int L = com1.Recv_length;
		string str = com1.read();
		LocalSenerDealdata(str, L, this);
	}
}


void Dataprocess(int L,string str,void * args)
{
	UWBsenser* P = (UWBsenser*)args;
	LocalSenerDealdata(str, L, P);
}


UCHAR UWBsenser::star(const char* szStr1, const char* szStr2)
{
	time_t t;
	t = time(0);
	
	//打开两个com口
	com1.setCallBack(Dataprocess,this);
	com2.setCallBack(Dataprocess, this);

	com1.Open(szStr1, 115200,0xA5,0x5A);
	com2.Open(szStr2, 115200,0xA5,0x5A);

	UCHAR S = 0;
	
	if (com1.comstate())
	{
		S = S | 1;
	}

	if (com2.comstate()) 
	{
		S = S | 1<<1;
	}

	CreateThread(NULL, 0, onlineChick, this, 0, NULL);
	return S;
}


