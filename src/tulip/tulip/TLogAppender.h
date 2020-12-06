/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogAppender.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Several appender classes definitions for Tulip TLog
**********************************************************************************/
#ifndef _TULIP_TLOG_APPENDER_H
#define _TULIP_TLOG_APPENDER_H


#include <set>
#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "tulip/TLogSynchronize.h"
#include "tulip/TLogLayout.h"
#include "tulip/TLogItem.h"
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@brief     Base class for all TLogXXAppender
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:45
 */
class TLogBaseAppender
{
public:
    virtual int    Pushback(TLogItem& item) = 0;
    virtual int    Pushback(TLogItemPtr item) = 0;
    virtual void   Flush() = 0;

    virtual void   SetLayout(TLogLayoutBase* layout = NULL);


    virtual inline void SetCacheLimit(uint32_t cacheLimit) {}
    virtual inline void SetCompress(bool bCompress) {}
    virtual inline void SetDelayTime(uint32_t hour) {}
    virtual inline void SetMaxSize(uint32_t maxSize) {}


    void SetAutoFlush(bool autoFlush) { m_autoFlush = autoFlush; }
    bool IsAutoFlush() { return m_autoFlush; }

public:
    static void                       s_Release();
public:
    static set<TLogBaseAppender*>     s_appenders;

protected:
    TLogBaseAppender();
    TLogBaseAppender(const TLogBaseAppender &app) { }
    virtual ~TLogBaseAppender();

    TLogMutex                m_appendMutex;
    TLogLayoutBase*          m_layout;
    bool                     m_autoFlush;
};


/**
 *@brief     Appender to output by udp
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:46
 */
class TLogUdpAppender: public TLogBaseAppender
{
public:
    virtual int         Pushback(TLogItem& item);
    virtual int         Pushback(TLogItemPtr item);
    virtual void        Flush() { }

public:
    static void                s_Release();
    static TLogBaseAppender*   s_GetAppender(string ip, int32_t port);
private:
    static TLogBaseAppender*    s_appender;
    static TLogMutex            s_appenderMutex;

protected:
    TLogUdpAppender(string ip, int32_t port);
    TLogUdpAppender(const TLogUdpAppender &app) { }
    virtual ~TLogUdpAppender();

private:
	bool Init();

private:
    string                      m_ip;
	int32_t                     m_port;
    sockaddr_in                 m_pin;
	int32_t                     m_sock_fd;
};


/**
 *@brief     output log to console
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:46
 */
class TLogConsoleAppender: public TLogBaseAppender
{
public:
    virtual int       Pushback(TLogItem& item);
    virtual int       Pushback(TLogItemPtr item);
    virtual void      Flush();

public:
    static TLogBaseAppender*   s_GetAppender();
    static void                s_Release();

protected:
    TLogConsoleAppender() { }
    TLogConsoleAppender(const TLogConsoleAppender &app) { }
    virtual ~TLogConsoleAppender() { }
private:
    static TLogBaseAppender*      s_appender;
    static TLogMutex              s_appenderMutex;
};


/**
 *@brief    struct used in TLogFileAppender for compress
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:47
 */
#define CHUNK 16384
typedef struct Parameter
{
    uint64_t         cacheLimit;
    char*            fileName;
} CompressParam;

/**
 *@brief     output log into file
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:47
 */
class TLogFileAppender: public TLogBaseAppender
{
public:
    virtual int    Pushback(TLogItem& item);
    virtual int    Pushback(TLogItemPtr item);
    virtual void   Flush();

public:
    static void               s_Release();
    static TLogBaseAppender*  s_GetAppender(const char *filename);

public:
    virtual inline void SetMaxSize(uint32_t maxSize);
    virtual inline void SetDelayTime(uint32_t hour);
    virtual inline void SetCompress(bool bCompress);
    virtual inline void SetCacheLimit(uint32_t cacheLimit);

protected:
    TLogFileAppender(const char * filename);
    TLogFileAppender(const TLogFileAppender &app) {}
    virtual ~TLogFileAppender();

private:
    static void* s_CompressHook(void *p);
private:
    static map<string, TLogBaseAppender*>            s_fileAppenders;
    static TLogMutex                                 s_appenderMapMutex;

private:
    int          Open();
    int          Close();
    void         RotateLog(time_t t);
    void         ResetRotateTime(time_t t);
    void         CompressLog(CompressParam *p);

    FILE*             m_file;
    string            m_fileName;

    time_t            m_rotateTime;
    uint32_t          m_nDelayHour;
    uint64_t          m_nCurSize;
    uint64_t          m_nMaxSize;
    uint64_t          m_nPos;
    uint64_t          m_nCacheLimit;
    bool              m_bCompress;
};


TULIP_END_NAMESPACE

#endif //_TULIP_TLOG_APPENDER_H

