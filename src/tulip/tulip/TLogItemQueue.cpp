/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogItemQueue.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementations for Tulip TLog data items queue data and operator interfaces
**********************************************************************************/
#include "tulip/TLogItemQueue.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <sys/stat.h>
#include <syslog.h>
#include <zlib.h>
#include <fcntl.h>
#include <errno.h>

TULIP_BEGIN_NAMESPACE


/**
 *@brief     enqueue operator for one TLog item
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:21
 */
bool TLogItemQueue::Enqueue(TLogItemInfoPtr itemInfo ) {
    m_condition.Lock();
    if (m_curQueueSize >= m_maxQueueSize) {
        m_condition.Unlock();
        return false;
    }

    TLogItemNode *node = new TLogItemNode(itemInfo);

    if (0 != m_curQueueSize) {
        node-> m_next = m_head;
        m_head-> m_prev = node;
        m_head = node;
    }
    else {
        m_head = m_tail = node;
        m_condition .Signal();
    }
    ++ m_curQueueSize;
    m_condition.Unlock();
    return true;
}


/**
 *@brief     dequeue operator for one TLog item
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:21
 */
TLogItemInfoPtr TLogItemQueue::Dequeue() {
    TLogItemInfoPtr info;

    m_condition.Lock();
    while ( 0 == m_curQueueSize && m_isBlock ) {
        m_condition.Wait();
    }

    if ( 0 == m_curQueueSize ) {
        m_condition.Unlock();
        return info;
    }

    info = m_tail->m_logInfo;

    if (1 == m_curQueueSize) {
        delete m_tail;
        m_head = m_tail = NULL;
    }
    else {
        m_tail = m_tail-> m_prev;
        delete m_tail-> m_next;
        m_tail -> m_next = NULL;
    }
    --m_curQueueSize;
    m_condition .Unlock();
    return info;
}


/**
 *@brief     clear all TLog items in queue
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:22
 */
void TLogItemQueue::Clear() {
    m_condition.Lock();
    while(m_head != NULL) {
        if(m_head->m_next != NULL) {
            m_head = m_head->m_next;
            delete m_head->m_prev;
        }
        else {
            delete m_head;
            break;
        }
    }
    m_head = m_tail = NULL;
    m_curQueueSize = 0;
    m_condition.Unlock();
}


uint32_t
TLogItemQueue::GetQueueSize() {
    uint32_t size;
    m_condition.Lock();
    size = m_curQueueSize;
    m_condition.Unlock();
    return size;
}

void TLogItemQueue::SetQueueLimit(uint32_t size) {
    m_condition.Lock();
    m_maxQueueSize = size;
    m_condition.Unlock();
}


void TLogItemQueue::SetNonBlock() {
    m_condition.Lock();
    m_isBlock = false;
    m_condition.Signal();
    m_condition.Unlock();
}



TULIP_END_NAMESPACE

