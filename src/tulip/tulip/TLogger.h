/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogger.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Main entrance tool interface for Tulip TLog use.
**********************************************************************************/
#ifndef _TULIP_TLOGGER_H
#define _TULIP_TLOGGER_H

#include "tulip/TLogThread.h"
#include "tulip/TLogAppender.h"
#include "tulip/TLogItemQueue.h"
#include "tulip/TLogItem.h"
#include "tulip/TLogSynchronize.h"
#include <vector>
#include <set>
#include <map>
#include <stdint.h>
#include <cinttypes>
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@brief     Definitions for several TLog Level No.
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:09
 */
const uint32_t TLOG_LEVELNO_DISABLE = 0;
const uint32_t TLOG_LEVELNO_FATAL   = 1;
const uint32_t TLOG_LEVELNO_ERROR   = 2;
const uint32_t TLOG_LEVELNO_WARN    = 3;
const uint32_t TLOG_LEVELNO_INFO    = 4;
const uint32_t TLOG_LEVELNO_DEBUG   = 5;
const uint32_t TLOG_LEVELNO_TRACE1  = 6;
const uint32_t TLOG_LEVELNO_TRACE2  = 7;
const uint32_t TLOG_LEVELNO_NOTSET  = 8;
const uint32_t TLOG_LEVELNO_COUNT   = 9;

/**
 *@brief     Class for main TLog use tool interfaces
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:12
 */
class TLogger {
public:
    friend class TLogThread;
private:
    TLogger(const char *loggerName, uint32_t levelNo = TLOG_LEVELNO_INFO, TLogger *parentLogger = NULL, bool bInheritIn = true);
    ~TLogger();
    TLogger(const TLogger &logger) { }


public:
    //get interface
    uint32_t            GetLevel() const { return m_loggerLevel; }
    const string&       GetLoggerName() const { return m_loggerName; }
    bool                GetInherit() const { return m_bInherit; }
    const TLogger*      GetParentLogger() const { return m_parent; }
    inline bool         IsLevelEnabled(uint32_t level) { return level <= m_loggerLevel; }

    //set interface
    void                SetLevelNo(uint32_t level);
    bool                SetInherit(bool bInherit);

    void                ClearAppenders();
    void                ResetAppender(TLogBaseAppender* appender);
    void                AddAppender(TLogBaseAppender* appender);

    //public Log interface
    void                Log(uint32_t levelNo, const char *format, ...) __attribute__((format(printf, 3, 4)));  // first parameter is this, so here __attribute__((format(printf,3,4)))
    void                Log(uint32_t levelNo, const char *filename, int line, const char* function, const char *format, ...) __attribute__((format(printf, 6, 7)));
    void                LogPureMessage(uint32_t level, const char *message);
    void                Flush();

private:
    void                SetLevelByParent(uint32_t parentLevelNo);
    void                Dump(TLogItemPtr item);

public:
    static TLogger*     s_GetRootLogger();
    static TLogger*     s_GetLogger(const char *name, bool bInherit = true);

    static void         s_Setup();
    static void         s_FlushAll();
    static void         s_Shutdown();

    static uint32_t     s_GetLogCount() { return s_logQueue.GetQueueSize(); }
    static void         s_SetLogQueueLimit(uint32_t size) { s_logQueue.SetQueueLimit(size); }
private:
    static TLogger*     s_GetLoggerInternal(const char *loggerName, bool bInherit = true);

public:
    static int                            s_MaxMsgLength;
    static string                         s_DefaultPatternStr;
private:
    static TLogItemQueue                  s_logQueue;
    static TLogThread                     s_logThread;

    static TLogger*                       s_rootLogger;
    static TLogMutex                      s_rootMutex;

    static map<string, TLogger*>*         s_loggerMap;
    static TLogMutex                      s_loggerMapMutex;

private:
    set<TLogBaseAppender*>              m_appenderSet;
    TLogMutex                           m_appenderMutex;

    vector<TLogger*>                    m_children;

    string                              m_loggerName;
    uint32_t                            m_loggerLevel;
    bool                                m_bLevelSet;
    TLogger*                            m_parent;
    bool                                m_bInherit;
};




TULIP_END_NAMESPACE

#endif //_TULIP_TLOGGER_H

