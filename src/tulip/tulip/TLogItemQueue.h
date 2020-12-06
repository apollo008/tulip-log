/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogItemQueue.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Definitions for Tulip TLog data items queue data and operator interfaces
**********************************************************************************/
#ifndef _TULIP_TLOG_ITEM_QUEUE_H
#define _TULIP_TLOG_ITEM_QUEUE_H


#include <memory>
#include <pthread.h>
#include <stdint.h>
#include "tulip/TLogSynchronize.h"
#include "tulip/TLogLayout.h"
#include "tulip/TLogItem.h"
#include <map>
#include <set>
#include <string>
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE


class TLogger;


/**
 *@brief     CLass for TLog item info
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:20
 */
class TLogItemInfo {
public:
    TLogItemInfo()
    : m_log(NULL)
    { }

public:
    TLogger*          m_log;
    TLogItemPtr       m_item;
};
typedef std::shared_ptr<TLogItemInfo> TLogItemInfoPtr;


/**
 *@brief     Class for TLog item Node in queue
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:20
 */
class TLogItemNode
{
public:
    TLogItemNode()
    : m_prev(NULL)
    , m_next(NULL)
    {
    }

    TLogItemNode(TLogItemInfoPtr info)
    : m_logInfo(info)
    , m_prev(NULL)
    , m_next(NULL)
    {
    }

public:
    TLogItemInfoPtr       m_logInfo;
    TLogItemNode*         m_prev;
    TLogItemNode*         m_next;
};


/**
 *@brief     Class for TLog item queue
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:21
 */
class TLogItemQueue
{
public:
    TLogItemQueue(uint32_t maxQueueSizeIn = 50000)
    : m_maxQueueSize(maxQueueSizeIn)
    , m_curQueueSize(0)
    , m_head(NULL)
    , m_tail(NULL)
    , m_isBlock(true)
    {

    }


    ~TLogItemQueue() { Clear(); }

public:
    TLogItemInfoPtr Dequeue();
    bool Enqueue(TLogItemInfoPtr itemInfo );


    void Clear();

    void SetQueueLimit(uint32_t size);
    uint32_t GetQueueSize();


    void SetNonBlock();
private:
    TLogCondition                m_condition;
    volatile uint32_t            m_maxQueueSize;
    uint32_t                     m_curQueueSize;
    TLogItemNode*                m_head;
    TLogItemNode*                m_tail;
    volatile bool                m_isBlock;
};





TULIP_END_NAMESPACE

#endif //_TULIP_TLOG_ITEM_QUEUE_H
