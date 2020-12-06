/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogThread.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Definition TLog thread to receive log content from other theads
**********************************************************************************/
#ifndef _TULIP_TLOG_THREAD_H
#define _TULIP_TLOG_THREAD_H

#include <stdint.h>
#include <pthread.h>
#include "tulip/TLogItemQueue.h"
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE


/**
 *@brief     tlog stat used in TLog thread
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:34
 */
enum _tlog_stat_t {
    tlog_init,
    tlog_started,
    tlog_terminated
};
typedef enum _tlog_stat_t tlog_stat_t;


/**
 *@brief     Class for TLog thread
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:34
 */
class TLogThread{
public:
    TLogThread();
    ~TLogThread() {}
public:
    bool Start(TLogItemQueue* pLogQueue);
    bool IsStarted();
    void Terminate();
    void Join();
    void Run();
protected:
    pthread_t                   m_tid;
    volatile tlog_stat_t        m_stat;
    TLogItemQueue*              m_logQueue;
};
typedef std::shared_ptr<TLogThread> TLogThreadPtr;

TULIP_END_NAMESPACE

#endif //_TULIP_TLOG_THREAD_H
