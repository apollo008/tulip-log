/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogItem.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementations for Tulip TLog Content items and operator interface
**********************************************************************************/
#include "tulip/TLogItem.h"

TULIP_BEGIN_NAMESPACE

/*static*/
const char* TLogItem::s_LevelName[] = {
        "DISABLE", "FATAL", "ERROR", "WARN",  "INFO", "DEBUG", "TRACE1", "TRACE2","NOTSET"
};


/**
 *@brief     static private method to obtain current time by format:yyyy-mm-dd HH:mm::SS
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:18
 */
void TLogItem::s_GetCurrentTime(char cur[], int length) {
    time_t n = time(NULL);

    struct tm * p = localtime(&n);

    snprintf(cur, length, "%04d-%02d-%02d %02d:%02d:%02d",
             p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
             p->tm_hour, p->tm_min, p->tm_sec);
}


/**
 *@brief     Construction method for class TLogItem
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:19
 */
TLogItem::TLogItem(const string&         loggerName,
         const string&                   msg,
         const uint32_t                       levelNo,
         const string&                   file  /* = "" */,
         const int&                           line  /* = 0  */,
         const string&                   func  /* = "" */)

        : m_loggerName(loggerName)
        , m_levelStr(s_LevelName[levelNo])
        , m_levelNo(levelNo)
        ,  m_message(msg)
        ,  m_file(file)
        , m_line(line)
        , m_function(func)
        , m_pid(getpid())
        , m_tid((long)syscall(SYS_gettid))
{
    s_GetCurrentTime(m_logTime, 20);
}








TULIP_END_NAMESPACE

