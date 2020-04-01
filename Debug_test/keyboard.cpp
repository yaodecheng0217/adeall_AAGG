#include"APP/app.h"
#include <conio.h>
#include<Windows.h>
#include<iostream>

#include"keyboard.h"
extern APP app;
int scanKeyboard()
{
	int ch;
	while (true)
	{
		if (_kbhit())
		{                 
			ch = _getch();
			std::cout << ch << std::endl;
			break;
		}
		Sleep(100);
	}
	return ch;
}

DWORD WINAPI Keyboard_control(void* prt)
{
	while (true)
	{
		switch (scanKeyboard())
		{
			case 27://z
		{
			exit(1);
			
		}
		case 122://z
		{
			app.Set_AUTO(1);	
		}
		break;
		case 120://x
		{
			app.Set_AUTO(0);
		}
		break;
		case 48://0
		{
			app.Set_Forward_motor(app.MODE::mode_abs, 0);
			printf("0!\n");
		}
		break;
		case 42://
		{
			
			printf("*\n");
		}
		break;
		case 46://
		{
			app.Set_Lift_motor(app.MODE::mode_abs, 0);
			printf(".\n");
		}
		break;
		case 97://
		{
			app.Set_Turn_motor(app.mode_inc, -5 / 57.3);
			printf(".\n");
		}
		break;
		case 100://
		{
			app.Set_Turn_motor(app.mode_inc,5/57.3);
			printf(".\n");
		}
		break;
		case 32://
		{
			app.Set_Acc_motor(app.MODE::mode_abs, 0);
			printf("stop!\n");
		}
		break;
		case 119://w
		{
			app.Set_Acc_motor(app.MODE::mode_abs,0.5);
			printf("Acc up!\n");
		}
		break;
		case 115://s
		{
			app.Set_Acc_motor(app.MODE::mode_abs, -0.5);
			printf("Acc Down!\n");
		}break;
		case 244:
		case 0: {
			switch (scanKeyboard())
			{
			case 72:
				app.Set_Lift_motor(app.MODE::mode_abs, 0.7);
				printf("up\n");
				break;
			case 75:
				app.Set_Forward_motor(app.MODE::mode_abs, -0.1);
				printf("left\n");
				break;
			case 77:
				app.Set_Forward_motor(app.MODE::mode_abs, 0.1);
				printf("right\n");
				break;
			case 80:
				app.Set_Lift_motor(app.MODE::mode_abs, -0.5);
				printf("down\n");
				break;
			default:
				break;
			}
		}
		break;

		default:
			break;
		}
	}
}

void StarKeyboard()
{
	CreateThread(NULL, 0, Keyboard_control, NULL, 0, NULL);
}

