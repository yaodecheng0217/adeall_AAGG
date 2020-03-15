/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-12 09:22:20
 * @LastEditTime: 2019-11-10 12:30:07
 * @LastEditors: Please set LastEditors
 */
#ifndef MUTEXLOCK_HPP_
#define MUTEXLOCK_HPP_

#ifdef _WIN32
#include <windows.h>
#else
#include <assert.h>
#include <mutex>
#endif
/**
 * @brief  多线程互斥锁模板, 利用C++析构函数特性,防止代码退出未释放锁
 * @note    //使用方法:
            //在要加锁的文件中，定义一个全局的 MutexLock ，
            //然后在要加锁的地方，定义 ScopeLocker。如果要限定代码范围用{}把要加锁的代码和自身扩起来。
            如:
           ```
           MutexLock  muLock;

           void fun1(){
                ScopeLocker lock( &muLock );
                ....  //critical area;
           }

           void fun2(){
                {

                ScopeLocker lock( &muLock );
                ....  //critical area;

                }
                ....  //other code;
            }
           ```
 */
#ifdef _WIN32
class MutexLock
{
public:
	MutexLock()
	{
		hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	}
	~MutexLock()
	{
	}
	void lock()
	{
		WaitForSingleObject(hEvent, INFINITE);  //等待对象为有信号状态
	}
	void unlock()
	{
		SetEvent(hEvent);
	}
private:
	HANDLE hEvent;  
};
#else
class MutexLock
{
public:
    MutexLock()
    {
        assert(pthread_mutex_init(&_mutex, NULL) == 0);
    }
    ~MutexLock()
    {
        pthread_mutex_destroy(&_mutex);
    }
    void lock()
    {
        assert(pthread_mutex_lock(&_mutex) == 0);
    }
    void unlock()
    {
        assert(pthread_mutex_unlock(&_mutex) == 0);
    }

private:
    pthread_mutex_t _mutex;
};
#endif
class ScopeLocker
{
public:
    ScopeLocker(MutexLock *mutex_lock)
    {
        //std::cout << "creat lock" << std::endl;
        _mutex_lock = mutex_lock;
        _mutex_lock->lock();
    }
    ~ScopeLocker()
    {
        if (_mutex_lock != NULL)
        {
            _mutex_lock->unlock();
        }
    }
    //也可主动调用
    void unlock()
    {
        _mutex_lock->unlock();
    }

private:
    MutexLock *_mutex_lock;
};

#endif
