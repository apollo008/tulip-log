/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogThread.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementation TLog thread to receive log content from other theads
**********************************************************************************/
#include "tulip/TLogThread.h"
#include "tulip/TLogger.h"

TULIP_BEGIN_NAMESPACE

/**
 *@brief     Construction method for TLogThread class
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:34
 */
TLogThread::TLogThread()
    : m_tid()
    , m_stat(tlog_init)
    , m_logQueue(NULL)
{

}


/**
 *@brief     hook method for pthread
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:35
 */
static void* log_hook(void *arg)
{
    TLogThread *thr = (TLogThread*)arg;
    thr->Run();
    return (void*)0;
}

/**
 *@brief     start log thread 
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:35
 */
bool TLogThread::Start(TLogItemQueue* logQueuePtr )
{
    if (tlog_init != m_stat) return false;

    if (NULL == logQueuePtr) return false;

    m_logQueue = logQueuePtr;

    if(pthread_create(&m_tid, NULL, log_hook, this) != 0) {
        m_logQueue = NULL;
        return false;
    }

    m_stat = tlog_started;
    return true;
}

bool TLogThread::IsStarted() {
    return tlog_started == m_stat;
}

/**
 *@brief     core Run method for thread
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:35
 */
void TLogThread::Run() {
    if (NULL != m_logQueue) {
        while (m_stat != tlog_terminated) {
            TLogItemInfoPtr info = m_logQueue ->Dequeue();
            if (nullptr != info) {
                info->m_log->Dump(info->m_item);
            }
        }
    }
}

/**
 *@brief     terminate TLogThread
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:36
 */
void TLogThread::Terminate() {
    m_stat = tlog_terminated;
    m_logQueue ->SetNonBlock();
}

void TLogThread::Join() {
    if( m_stat != tlog_init) {
        pthread_join(m_tid, NULL);
    }
    m_stat = tlog_init;
}

TULIP_END_NAMESPACE
