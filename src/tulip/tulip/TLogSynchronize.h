/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogSynchronize.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    It defines some synchronize inteface used in Tulip about lock and conditions
**********************************************************************************/
#ifndef _TULIP_TLOGSYNCHRONIZE_H
#define _TULIP_TLOGSYNCHRONIZE_H

#include <pthread.h>
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE


/**
 *@brief     implementation class for condition variable
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:25
 */
class TLogCondition
{
public:
    TLogCondition();
    ~TLogCondition();
public:
    void Lock();
    void Unlock();
    void Broadcast();
    void Signal();
    void Wait();
private:
    pthread_cond_t             m_cond;
    pthread_mutex_t            m_lock;
};





/**
 *@brief     implementation class for mutex lock
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:25
 */
class TLogMutex {
public:
    inline TLogMutex() { ::pthread_mutex_init(&m_mutex, NULL); }
    inline ~TLogMutex() { ::pthread_mutex_destroy(&m_mutex); }
    inline void lock() { ::pthread_mutex_lock(&m_mutex); }
    inline void unlock() { ::pthread_mutex_unlock(&m_mutex); }
private:
    TLogMutex& operator=(const TLogMutex &m);
    TLogMutex(const TLogMutex& m);
private:
    pthread_mutex_t m_mutex;
};

class TLogScopedLock
{
public:
    inline TLogScopedLock(TLogMutex& mutex)
    : m_mutex(mutex) {
        m_mutex.lock();
    }

    inline ~TLogScopedLock() { m_mutex.unlock(); }
private:
    TLogMutex&       m_mutex;
};

// read-writer locker for pthread_rwlock_t
class TLogRWMutex
{
public:
    inline TLogRWMutex() {
        ::pthread_rwlock_init(&m_mutex, NULL);
    }
    inline ~TLogRWMutex() {
        ::pthread_rwlock_destroy(& m_mutex);
    }

public:
    inline void wrlock() {
        ::pthread_rwlock_wrlock(&m_mutex);
    }
    inline void rdlock() {
        ::pthread_rwlock_rdlock(&m_mutex);
    }

    inline void unlock() {
        ::pthread_rwlock_unlock(& m_mutex);
    }

private:
    TLogRWMutex& operator=(const TLogMutex &m);
    TLogRWMutex(const TLogMutex& m);
private:
    pthread_rwlock_t    m_mutex;
};

//scoped rwlock for read
class TLogReadScopedLock
{
public:
    inline TLogReadScopedLock(TLogRWMutex& mutex)
    : m_mutex(mutex) {
        m_mutex.rdlock();
    }

    inline ~TLogReadScopedLock() {
        m_mutex.unlock();
    }
private:
    TLogRWMutex&  m_mutex;
};

//scoped rwlock for write
class TLogWriteScopedLock
{
public:
    inline TLogWriteScopedLock(TLogRWMutex& mutex)
            : m_mutex(mutex) {
        m_mutex.wrlock();
    }

    inline ~TLogWriteScopedLock() {
        m_mutex.unlock();
    }
private:
    TLogRWMutex&  m_mutex;
};



TULIP_END_NAMESPACE

#endif //_TULIP_TLOGSYNCHRONIZE_H
