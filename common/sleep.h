/*
 * @Author: Yaodecheng
 * @Date: 2020-03-15 11:47:50
 * @LastEditTime: 2020-03-15 12:05:17
 * @LastEditors: Yaodecheng
 */
#ifdef _WIN32
#include <windows.h>
#else
        usleep(1000 * 1000);
#endif



void _sleep_ms(uint32_t ms)
{
#ifdef _WIN32
        Sleep(ms);
#else
        usleep(ms * 1000);
#endif
}

