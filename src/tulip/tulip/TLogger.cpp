/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogger.cpp
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementation for main entrance tool interface for Tulip TLog use.
**********************************************************************************/
#include "tulip/TLogger.h"
#include "tulip/TLogUtil.h"
#include "tulip/TLogItem.h"
#include <iostream>

TULIP_BEGIN_NAMESPACE


/*static */
int TLogger::s_MaxMsgLength = 4096;      // max log message content length, it will be cut if length of log message is more than this limit.

/*static */
string TLogger::s_DefaultPatternStr = "[%%d] [%%t,%%F:%%n -- %%f() %%l] [%%m]";    //one useful and beautiful pattern 

/*static */
TLogItemQueue          TLogger::s_logQueue;
/*static */
TLogThread             TLogger::s_logThread;

/*static */
TLogger*               TLogger::s_rootLogger = NULL;
/*static */
TLogMutex              TLogger::s_rootMutex;

/*static */
map<string, TLogger*>* TLogger::s_loggerMap = new map<string, TLogger*>();
/*static */
TLogMutex              TLogger::s_loggerMapMutex;


/**
 *@brief     Construction for class TLogger
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:12
 */
TLogger::TLogger(const char*             loggerName,
                 uint32_t                levelNo        /* = TLOG_LEVELNO_INFO */,
                 TLogger*                parentLogger   /* = NULL  */,
                 bool                    bInheritIn     /* = true */)
: m_loggerName(loggerName)
, m_loggerLevel(levelNo)
, m_bLevelSet(false)
, m_parent(parentLogger)
, m_bInherit(bInheritIn)
{

}

/**
 *@brief     factory method to obtain root logger
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:12
 */
TLogger* TLogger::s_GetRootLogger()
{
    TLogScopedLock lock(s_rootMutex);
    if (s_rootLogger == NULL) {
        s_rootLogger = new TLogger("", TLOG_LEVELNO_INFO);
    }
    return s_rootLogger;
}

/**
 *@brief     factory method to obtain logger by logger name
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:12
 */
TLogger* TLogger::s_GetLogger(const char *loggerName, bool bInherit ) {
    TLogScopedLock lock(s_loggerMapMutex);
    if(NULL == s_loggerMap) {
        s_loggerMap = new map<string, TLogger*>();
    }
    return s_GetLoggerInternal(loggerName, bInherit);
}


/**
 *@brief     flush all logger content, it is often used before release
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:12
 */
void TLogger::s_FlushAll() {
    {
        TLogScopedLock lock(s_loggerMapMutex);
        if(NULL != s_loggerMap){
            for (map<string, TLogger*>::iterator it = s_loggerMap->begin(); it != s_loggerMap->end(); it++) {
                it->second->Flush();
            }
        }
    }
    {
        TLogScopedLock lock(s_rootMutex);
        if(NULL != s_rootLogger)
            s_rootLogger->Flush();
    }
}

/**
 *@brief     global setup for all TLoggers, it often starts TLog thread to receive any logger content
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:14
 */
void TLogger::s_Setup() {
    if(!s_logThread.IsStarted()) {
        s_logThread.Start(&s_logQueue);
    }
}

/**
 *@brief     shutdown all TLoggers
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:14
 */
void TLogger::s_Shutdown() {
    if(s_logThread.IsStarted()) {
        s_logThread.Terminate();
        s_logThread.Join();       
    }

    {
        TLogScopedLock tmpLock(s_rootMutex);
        if (NULL != s_rootLogger) {
            delete s_rootLogger;
            s_rootLogger = NULL;
        }
    }

    {
        TLogScopedLock lock(s_loggerMapMutex);
        if(NULL != s_loggerMap){
            for (map<string, TLogger*>::const_iterator it =  s_loggerMap->begin(); it != s_loggerMap->end(); it++) {
                delete ((*it).second);
            }
            s_loggerMap->clear();
            delete s_loggerMap;
            s_loggerMap = NULL;
        }
    }
    s_logQueue.Clear();
    TLogBaseAppender::s_Release();
    TLogConsoleAppender::s_Release();
    TLogUdpAppender::s_Release();
    TLogFileAppender::s_Release();
}

/**
 *@brief     Deconstruction for class TLogger
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:15
 */
TLogger::~TLogger()
{
    ClearAppenders();
    m_children.clear();
}



void TLogger::SetLevelNo(uint32_t levelNo)   {
    if(levelNo < TLOG_LEVELNO_COUNT) {
        m_loggerLevel = (levelNo == TLOG_LEVELNO_NOTSET && m_bInherit && m_parent)? m_parent->GetLevel() : levelNo;
        if(m_loggerLevel == TLOG_LEVELNO_NOTSET) {
            m_loggerLevel = TLOG_LEVELNO_INFO;
        }
        m_bLevelSet = (TLOG_LEVELNO_NOTSET != levelNo);
        for (size_t t = 0; t < m_children.size(); ++t) {
            m_children[t]->SetLevelByParent(m_loggerLevel);
        }
    }
}

void TLogger::SetLevelByParent(uint32_t parentLevelNo)   {
    if(parentLevelNo < TLOG_LEVELNO_COUNT && m_bInherit && !m_bInherit) {
        m_loggerLevel = (parentLevelNo == TLOG_LEVELNO_NOTSET)? TLOG_LEVELNO_INFO : parentLevelNo;
        for (size_t t = 0; t < m_children.size(); t++) {
            m_children[t]->SetLevelByParent(m_loggerLevel);
        }
    }
}


bool TLogger::SetInherit(bool bInherit) {
    m_bInherit = bInherit;
    if (m_bLevelSet) return true;

    if(m_bInherit) {
        if (!m_parent) {
            cerr << "Invalid log level set, Please check!!!" << endl;
            return false;
        }
        m_loggerLevel = m_parent->GetLevel();
    }
    else {
        m_loggerLevel = TLOG_LEVELNO_INFO;
    }

    for (size_t t = 0; t < m_children.size(); t++) {
        m_children[t]->SetLevelByParent(m_loggerLevel);
    }

    return true;
}

void TLogger::ResetAppender(TLogBaseAppender* appender) {
    if (NULL != appender) {
        TLogScopedLock lock(m_appenderMutex);
        m_appenderSet.clear();
        m_appenderSet.insert(appender);
    }
}

void TLogger::AddAppender(TLogBaseAppender* appender) {
    if (NULL != appender) {
        TLogScopedLock lock(m_appenderMutex);
        if (m_appenderSet.find(appender) == m_appenderSet.end()) {
            m_appenderSet.insert(appender);
        }
    }
}

void TLogger::ClearAppenders()
{
    TLogScopedLock lock(m_appenderMutex);
    m_appenderSet.clear();
}

void TLogger::Log(uint32_t levelNo, const char * file, int line, const char * func, const char* format, ...)
{
    if (__builtin_expect((!IsLevelEnabled(levelNo)),1))
        return;

    char buf[TLogger::s_MaxMsgLength];
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(buf, TLogger::s_MaxMsgLength, format, arg_ptr);
    va_end(arg_ptr);
    string msg(buf);
    TLogItemPtr item = make_shared<TLogItem>(m_loggerName, msg, levelNo, string(file), line, string(func));
    TLogItemInfoPtr info = make_shared<TLogItemInfo>();
    info->m_log = this;
    info->m_item = item;
    TLogger::s_logQueue.Enqueue(info);
}

void TLogger::Log(uint32_t levelNo, const char* format,  ...)
{
    if (__builtin_expect((!IsLevelEnabled(levelNo)),1))
        return;

    char buf[ TLogger::s_MaxMsgLength];
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(buf , TLogger::s_MaxMsgLength, format, arg_ptr);
    va_end(arg_ptr);
    string msg(buf );
    TLogItemPtr item = make_shared<TLogItem >(m_loggerName, msg, levelNo);
    TLogItemInfoPtr info = make_shared<TLogItemInfo>();
    info->m_log = this;
    info->m_item = item;
    s_logQueue.Enqueue(info);
}

void TLogger::LogPureMessage(uint32_t level, const char *message) {
    if (__builtin_expect((!IsLevelEnabled(level)),1)) {
        return;
    }

    string msg(message);
    TLogItemPtr item = make_shared<TLogItem>(m_loggerName, msg, level);
    TLogItemInfoPtr info = make_shared<TLogItemInfo>();
    info->m_log = this;
    info->m_item = item;
    s_logQueue.Enqueue(info);
}

void TLogger::Flush()
{
    TLogScopedLock lock(m_appenderMutex);
    for (set<TLogBaseAppender*>::const_iterator it =  m_appenderSet.begin(); it != m_appenderSet.end(); it++) {
        (*it)->Flush();
    }
}

void TLogger::Dump(TLogItemPtr item) {
    if (nullptr == item) return;

    if (m_bInherit &&  NULL != m_parent ) {
        m_parent->Dump(item);
    }

    if (item->m_loggerName.empty()) {
        item->m_loggerName = m_loggerName;
    }
    TLogScopedLock lock(m_appenderMutex);
    for (set<TLogBaseAppender*>::const_iterator it = m_appenderSet.begin(); it != m_appenderSet.end(); ++it ) {
        (*it)->Pushback(item);
    }
}

TLogger* TLogger::s_GetLoggerInternal(const char* loggerNameIn, bool bInherit )
{
    string loggerName = loggerNameIn;
    map<string, TLogger*>::iterator it;
    if (NULL != s_loggerMap && (it = s_loggerMap -> find(loggerName)) != s_loggerMap->end()) {
        return it -> second;
    }
    string parentLoggerName;
    size_t inx = loggerName.find_last_of('.');
    TLogger* parentLogger = NULL;
    if (inx == string::npos) {
        parentLogger = TLogger::s_GetRootLogger();
    }
    else {
        parentLoggerName = loggerName.substr(0, inx);
        parentLogger = s_GetLoggerInternal(parentLoggerName.c_str());
    }
    TLogger* target = new TLogger(loggerNameIn, parentLogger->GetLevel(), parentLogger, bInherit);
    parentLogger->m_children.push_back(target);
    s_loggerMap->insert(pair<string, TLogger*>(loggerName, target));
    return target;
}










TULIP_END_NAMESPACE

