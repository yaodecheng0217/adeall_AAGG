/*
 * @Descripttion: 
 * @version: 
 * @Author: Yaodecheng
 * @Date: 2019-10-19 10:45:26
 * @LastEditors: Yaodecheng
 * @LastEditTime: 2020-03-24 09:45:44
 */

#include "thread_base.h"
#ifdef _WIN32
DWORD WINAPI thread_base::threadfun(void *ptr)
#else
void *thread_base::threadfun(void *ptr)
#endif
{
    thread_base *p = (thread_base *)ptr;
    RunFun fun = p->_fun;
    void *context = p->_context;
    p->L.unlock();
    fun(context);
    return 0;
}

thread_base::thread_base(RunFun f, void *context)
    : _fun(f), _context(context)
{
    L.lock();
#ifdef _WIN32
    Thandle = CreateThread(NULL, 0, threadfun, this, 0, NULL);
    if (Thandle == NULL)
#else
    if (pthread_create(&recvThread, NULL, threadfun, this))
#endif
    {
        printf("Error creating readThread.\n");
        L.unlock();
    }
    L.lock();
#ifdef _WIN32
    CloseHandle(Thandle);
#endif
}

thread_base::~thread_base()
{
    //CloseHandle(Thandle);
}
