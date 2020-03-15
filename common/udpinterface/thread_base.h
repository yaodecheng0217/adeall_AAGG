/*
 * @Descripttion: 通用线程库，还未完善仅仅提供简单线程创建
 * @version: v1.0
 * @Author: Yaodecheng
 * @Date: 2019-10-19 10:32:13
 * @LastEditors: Yaodecheng
 * @LastEditTime: 2020-03-12 15:48:56
 */
#ifndef THREAD_BASE_H
#define THREAD_BASE_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include <sys/types.h>
#include"mutexlock.hpp"
/*
   thread_base thread(RunFunction);
*/
class thread_base
{
	typedef void *(*RunFun)(void *);//线程运行函数
private:
#ifdef _WIN32
	static DWORD WINAPI threadfun(void *);
#else
	pthread_t recvThread;
	static void *threadfun(void *);
#endif
	RunFun _fun;
	void *_context;
    MutexLock L;
public:
	thread_base(RunFun, void *);
	~thread_base();
};

#endif