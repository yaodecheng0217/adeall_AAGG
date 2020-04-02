#include "ComCTR.h"

DWORD WINAPI  Comrecv(void* args);
DWORD WINAPI  Comrecv2(void* args);
ComCTR::ComCTR()
{
	
}

void ComCTR::Open(const char* szStr, int BaudRate)
{
	InitializeCriticalSection(&m_csCommunicationSync);

	comfd = Serialinit(szStr, BaudRate);
	if (comfd == INVALID_HANDLE_VALUE)
	{
		//cout << "�򿪴���ʧ��!" << endl;
	}
	else
		CreateThread(NULL, 0, Comrecv, this, 0, NULL);

}
void ComCTR::testmodeOn()
{
	testmode = true;
}
void ComCTR::Open(const char* szStr, int BaudRate, char endstr)
{
	endF = endstr;
	InitializeCriticalSection(&m_csCommunicationSync);
	comfd = Serialinit(szStr, BaudRate);
	if (comfd == INVALID_HANDLE_VALUE)
	{
		cout <<"------com."<< "�򿪴���ʧ��!" << endl;
	}
	else
	{
		cout << "------com." << "open threath" << endl;
		CreateThread(NULL, 0, Comrecv, this, 0, NULL);

	}
}
void ComCTR::Open(const char* szStr, int BaudRate, char endstr,char endstr2)
{
	endF = endstr;
	endF2 = endstr2;
	InitializeCriticalSection(&m_csCommunicationSync);
	comfd = Serialinit(szStr, BaudRate);
	if (comfd == INVALID_HANDLE_VALUE)
	{
		cout << "------com." << "�򿪴���ʧ��!" << endl;
	}
	else
	{
		cout << "------com." << "open threath" << endl;
		CreateThread(NULL, 0, Comrecv2, this, 0, NULL);

	}
}

HANDLE ComCTR::Serialinit(const char* szStr, int BaudRate) {

	//const char temp[5]="COM4";


	WCHAR wszClassName[60];//5


	memset(wszClassName, 0, sizeof(wszClassName));

	MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName,

		sizeof(wszClassName) / sizeof(wszClassName[0]));
	
	HANDLE hCom1 = CreateFile(szStr,//COM1��

		GENERIC_READ | GENERIC_WRITE, //��������д

		0, //��ռ��ʽ

		NULL,

		OPEN_EXISTING, //�򿪶����Ǵ���

		0, //0ͬ����ʽ,0�첽��ʽ

		NULL);

	

	//if (hCom1 == (HANDLE)-1)

	if (hCom1 == INVALID_HANDLE_VALUE)

	{

		cout << "------com.";
		printf(szStr);
		printf(" is failed!");
		cout << " error = "<< (int)GetLastError() << endl;;
		return FALSE;

	}

	else

	{
		printf(szStr);
		printf(" is ok��\n");

	}



	SetupComm(hCom1, 1024, 1024); //���뻺����������������Ĵ�С����1024

	COMMTIMEOUTS TimeOuts;

	//�趨����ʱ

	TimeOuts.ReadIntervalTimeout = 10;

	TimeOuts.ReadTotalTimeoutMultiplier = 10;//5000;

	TimeOuts.ReadTotalTimeoutConstant = 50; //5000;

	//�趨д��ʱ

	TimeOuts.WriteTotalTimeoutMultiplier = 20;

	TimeOuts.WriteTotalTimeoutConstant = 20;

	SetCommTimeouts(hCom1, &TimeOuts); //���ó�ʱ

	DCB dcb;

	GetCommState(hCom1, &dcb);

	dcb.BaudRate = BaudRate; //������Ϊ1152

	dcb.ByteSize = 8; //ÿ���ֽ���8λ

	dcb.Parity = NOPARITY; //����żУ��λ

	dcb.StopBits = ONESTOPBIT; //1��ֹͣλ

	SetCommState(hCom1, &dcb);

	init = true;
	return hCom1;
}

bool ComCTR::WriteData(unsigned char* pData, unsigned int length)
{
    BOOL   bResult = TRUE;
	DWORD  BytesToSend = length;
	if (comfd == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);
	/** �򻺳���д��ָ���������� */
	bResult = WriteFile(comfd, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(comfd, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}
	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);
	//cout << "ok" << endl;
	return true;
}


bool ComCTR::ReadChar(char& cRecved)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 1;
	if (comfd == INVALID_HANDLE_VALUE)
	{	return false;
	}
	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);
	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(comfd, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */

		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(comfd, PURGE_RXCLEAR | PURGE_RXABORT);

		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}
	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);
	return (BytesRead == 1);
}

string ComCTR::read()
{
	string a = recvString;
	if(dataproces!=NULL)
	dataproces(Recv_length,a,up);
	Recv_length = 0;
	return a;
}

void ComCTR::setCallBack(dataFun P,void *UUP)
{
	dataproces = P;
	up = UUP;
}

bool ComCTR::comstate()
{
		return init;
}



/*���ڽ����߳�*/
DWORD WINAPI  Comrecv(void* args)
{
	ComCTR* P = (ComCTR*)args;
	//char a[1001];
	char b;
	while (true)
	{
		while (P->Recv_length != 0)
		{
			Sleep(1);
		};
		P->recvString = "";
	
		for (size_t i = 0; i < 2000; )
		{	
			if (P->ReadChar(b))
			{
				if (b == P->endF)
					break;	
				i++;
				if (P->testmode)
				cout << (int)b<<"_";
				printf("%2X ", b);

				P->recvString = P->recvString + b;
			}
			else
			{
				Sleep(50);
			};
		}
		printf("\n");
		P->Recv_length = P->recvString.size();
		P->read();
		//cout << P->recvString << endl;
		//****���Բ���
		/*if (P->testmode)
		{
			cout << endl;
		}		
		if (P->testmode)
		{
			P->read();
		}	*/	
	}
}
/*���ڽ����߳�*/
DWORD WINAPI  Comrecv2(void* args)
{
	ComCTR* P = (ComCTR*)args;
	//char a[1001];
	char b;
	while (true)
	{
		while (P->Recv_length != 0)
		{
			Sleep(1);
		};
		P->recvString = "";

		for (size_t i = 0; i < 2000; )
		{
			if (P->ReadChar(b))
			{
				if (b == P->endF)
				{
					P->ReadChar(b);
					if (b == P->endF2)
					{
						break;
					}
					else
					{
						i++;
						P->recvString = P->recvString +  P->endF;
					}
				}
					
				i++;
				if (P->testmode)
					cout << (int)b << "_";
				P->recvString = P->recvString + b;
			}
			else
			{
				Sleep(50);
			};
		}
		P->Recv_length = P->recvString.size();
		P->read();

		//cout << P->recvString << endl;
		//****���Բ���
		/*if (P->testmode)
		{
			cout << endl;
		}
		if (P->testmode)
		{
			P->read();
		}*/
	}
}