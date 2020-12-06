/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogAppender.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Several appender classes implementation for Tulip TLog
**********************************************************************************/
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <zlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tulip/TLogAppender.h"
#include "tulip/TLogger.h"
#include "tulip/TLogItem.h"

TULIP_BEGIN_NAMESPACE


/*static */
set<TLogBaseAppender*> TLogBaseAppender::s_appenders;

/**
 *@brief     Construction method for class TLogBaseAppender
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:56
 */
TLogBaseAppender::TLogBaseAppender()
{
    m_layout = new TLogBasicLayout();
    m_autoFlush = false;
}

/**
 *@brief     deconstruction method for class TLogBaseAppender
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:57
 */
TLogBaseAppender::~TLogBaseAppender()
{
    if(m_layout) {
        delete m_layout;
        m_layout = NULL;
    }
}

/**
 *@brief     set new layout
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:57
 */
void TLogBaseAppender::SetLayout(TLogLayoutBase* layout /*=NULL*/) {
    if (layout != m_layout) {
        TLogLayoutBase *oldLayout = m_layout;
        m_layout = (layout == NULL) ? new TLogBasicLayout() : layout;
        if(oldLayout)
            delete oldLayout;
    }
}

/**
 *@brief     release memory for all appenders
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:56
 */
void TLogBaseAppender::s_Release()
{
    for (set<TLogBaseAppender*>::iterator i = s_appenders.begin(); i != s_appenders.end(); i++)
    {
        if (NULL != *i)
            delete (*i);
    }
    s_appenders.clear();
}

TLogBaseAppender* TLogConsoleAppender::s_appender = NULL;
TLogMutex TLogConsoleAppender::s_appenderMutex;

TLogBaseAppender* TLogConsoleAppender::s_GetAppender()
{
    TLogScopedLock lock(s_appenderMutex);
    if (NULL == s_appender) {
        s_appender = new TLogConsoleAppender();
        s_appenders.insert(s_appender);
    }
    return s_appender;
}

int TLogConsoleAppender::Pushback(TLogItem& item)
{
    if(m_layout == NULL) {
        m_layout = new TLogBasicLayout();
    }
    string formatedStr = m_layout->Place(item);
    int wcnt = 0;
    {
        TLogScopedLock lock(m_appendMutex);
        wcnt = fwrite(formatedStr.data(), 1, formatedStr.length(), stdout);
        if (m_autoFlush || item.m_levelNo < TLOG_LEVELNO_INFO)
            fflush(stdout);
    }
    return ((size_t)wcnt == formatedStr.length())? formatedStr.length() : 0;
}

int TLogConsoleAppender::Pushback(TLogItemPtr item)
{
    if (nullptr == item) {
        return 0;
    }

    if(m_layout == NULL) {
        m_layout = new TLogBasicLayout();
    }

    string formatedStr = m_layout->Place(item);
    int wCnt = 0;
    {
        TLogScopedLock lock(m_appendMutex);
        wCnt = fwrite(formatedStr.data(), 1, formatedStr.length(), stdout);
        if (m_autoFlush || item ->m_levelNo < TLOG_LEVELNO_INFO)
            fflush(stdout);
    }
    return ((size_t)wCnt == formatedStr.length())? formatedStr.length() : 0;
}

void TLogConsoleAppender::Flush()
{
    fflush(stdout);
}

void TLogConsoleAppender::s_Release()
{
    s_appender = NULL;
}

TLogBaseAppender* TLogUdpAppender::s_appender = NULL;
TLogMutex TLogUdpAppender::s_appenderMutex;

TLogUdpAppender::TLogUdpAppender(string ip, int32_t port)
: m_ip(ip)
, m_port(port)
{
	Init();
}

TLogUdpAppender::~TLogUdpAppender()
{
    close(m_sock_fd);
}

void TLogUdpAppender::s_Release() {
    s_appender = NULL;
}

TLogBaseAppender* TLogUdpAppender::s_GetAppender(string ip, int32_t port)
{
	TLogScopedLock lock(s_appenderMutex);
	if (NULL == s_appender) {
		s_appender = new TLogUdpAppender(ip,port);
		s_appenders.insert(s_appender);
	}
	return s_appender;
}


int TLogUdpAppender::Pushback(TLogItem & item)
{
	if(m_layout == NULL)
		m_layout = new TLogBasicLayout();

	string formatedStr = m_layout->Place(item);
	int wCnt = 0;
	{
		TLogScopedLock lock(m_appendMutex);
		wCnt = write(m_sock_fd,formatedStr.data(), formatedStr.length());
	}
	return ((size_t)wCnt == formatedStr.length())? formatedStr.length() : 0;
}

int TLogUdpAppender::Pushback(TLogItemPtr item)
{
	if(nullptr == item) {
	    return 0;
	}
	return Pushback(*item);
}


bool TLogUdpAppender::Init() {
	bzero(&m_pin, sizeof(m_pin));
	m_pin.sin_family = AF_INET;
	m_pin.sin_port = htons(m_port);
	if (inet_pton(AF_INET, m_ip.c_str(), &m_pin.sin_addr)<=0) {
		return false;
	}
	m_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == m_sock_fd) {
		return false;
	}

	if (connect(m_sock_fd,(struct sockaddr*)&m_pin,sizeof(m_pin)) == -1) {
		return false;
	}

    return true;
}


map<string, TLogBaseAppender*>  TLogFileAppender::s_fileAppenders;
TLogMutex                       TLogFileAppender::s_appenderMapMutex;

TLogFileAppender::TLogFileAppender(const char * fileName)
: m_fileName(fileName)
{
    m_file = NULL;
    m_rotateTime = 0;
    m_nDelayHour = 0;
    m_nCurSize = 0;
    m_nMaxSize = 0;
    m_nPos = 0;
    m_nCacheLimit = 0;
    m_bCompress = false;
    Open();
}

TLogFileAppender::~TLogFileAppender()
{
    if (m_file) {
        Close();
    }
    m_file = NULL;
}

TLogBaseAppender* TLogFileAppender::s_GetAppender(const char *filenameIn)
{
    string filename = filenameIn;
    TLogScopedLock lock(s_appenderMapMutex);
    map<string, TLogBaseAppender*>::iterator it = s_fileAppenders.find(filename);
    if (s_fileAppenders.end() == it) {
        TLogBaseAppender* tmpAppender = new TLogFileAppender(filenameIn);
        s_fileAppenders[filename] = tmpAppender;
        s_appenders.insert(tmpAppender);
        return tmpAppender;
    }
    else {
        return it->second;
    }
}

int TLogFileAppender::Pushback(TLogItem& item) {
    if(!m_file) {
        return 0;
    }
    if(m_layout == NULL) {
        m_layout = new TLogBasicLayout;
    }
    string formatedStr = m_layout->Place(item);
    int wCnt = 0;
    {
        TLogScopedLock lock(m_appendMutex);
        if((m_nMaxSize > 0 && m_nCurSize + formatedStr.length() >= m_nMaxSize) || (m_rotateTime > 0 && time(NULL) >= m_rotateTime)) {
            RotateLog(time(NULL));
        }
        wCnt = fwrite(formatedStr.data(), 1, formatedStr.length(), m_file);
        if (m_autoFlush || m_nMaxSize > 0 || item.m_levelNo < TLOG_LEVELNO_INFO)
            fflush(m_file);
        m_nCurSize += wCnt;
        if(m_nCacheLimit > 0 && (m_nCurSize - m_nPos) > m_nCacheLimit) {
            posix_fadvise(fileno(m_file), 0, 0, POSIX_FADV_DONTNEED);
            m_nPos = m_nCurSize;
        }
    }
    return ((size_t)wCnt == formatedStr.length())? formatedStr.length() : 0;
}

int TLogFileAppender::Pushback(TLogItemPtr item) {
    if (nullptr == item) {
        return 0;
    }
    return Pushback(*item);
}


void TLogFileAppender::Flush()
{
    if(m_file) {
        fflush(m_file);
    }
}

int TLogFileAppender::Open() {
    m_file = fopen64(m_fileName.c_str(), "a+");
    if (m_file) {
        struct stat64 stbuf;
        if (fstat64(fileno(m_file), &stbuf) == 0) {
            m_nCurSize = stbuf.st_size;
            m_nPos = 0;
        }
        return 1;
    }
    else {
        fprintf(stderr, "Open log file error : %s\n",  m_fileName.c_str());
        return 0;
    }
}

void TLogFileAppender::s_Release()
{
    s_fileAppenders.clear();
}

int TLogFileAppender::Close() {
    int ret = -1;
    if(m_file)
        ret = fclose(m_file);
    m_file = NULL;
    return ret;
}

void TLogFileAppender::RotateLog(time_t t)
{
    if(m_nCacheLimit > 0)
        posix_fadvise(fileno(m_file), 0, 0, POSIX_FADV_DONTNEED);
    Close();
    char *oldLogFile = new char[512];
    char oldZipFile[512];

    // time_t t; 
    // time(&t);
    struct tm *tm, tim;
    if(t >=  m_rotateTime)
    {
        tm = ::localtime_r((const time_t*)&m_rotateTime, &tim);
    }
    else
    {
        tm = ::localtime_r((const time_t*)&t, &tim);
    }
    sprintf(oldLogFile, "%s.%04d%02d%02d%02d%02d%02d", m_fileName.c_str(), tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    sprintf(oldZipFile, "%s.gz",oldLogFile);
    int subCount = 1;
    while(access(oldLogFile, R_OK) == 0 || (m_bCompress && access(oldZipFile, R_OK) == 0))
    {
        sprintf(oldLogFile, "%s-%d", oldLogFile, subCount);
        sprintf(oldZipFile, "%s.gz",oldLogFile);
        subCount++;
    }
    rename(m_fileName.c_str(), oldLogFile);
    if(m_rotateTime > 0 && t >= m_rotateTime)
        ResetRotateTime(t);
    Open();
    if(m_bCompress)
    {
        CompressParam  *p = new CompressParam ();
        p->fileName = oldLogFile;
        p->cacheLimit = m_nCacheLimit;
        CompressLog(p);
    }
    else
        delete[] oldLogFile;
}

void TLogFileAppender::ResetRotateTime(time_t t)
{
    uint32_t delayseconds = m_nDelayHour*3600;
    if(t - m_rotateTime > delayseconds) {
        t = m_rotateTime + (t-m_rotateTime)/delayseconds*delayseconds;
    }
    m_rotateTime = (t + delayseconds)/3600*3600;
}

void TLogFileAppender::SetMaxSize(uint32_t maxSize)
{
    m_nMaxSize = (uint64_t)maxSize * 1024 * 1024;
}

void TLogFileAppender::SetCacheLimit(uint32_t cacheLimit)
{
    m_nCacheLimit = (uint64_t)cacheLimit * 1024 * 1024;
}

void TLogFileAppender::SetDelayTime(uint32_t hour)
{
    m_nDelayHour = hour;
    if(hour > 0) {
        ResetRotateTime(time(NULL));
    }
}

void TLogFileAppender::SetCompress(bool bCompress)
{
    m_bCompress = bCompress;
}

void TLogFileAppender::CompressLog(CompressParam *p)
{
    pthread_t pThread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&pThread, &attr, TLogFileAppender::s_CompressHook, (void*)p);
}

void* TLogFileAppender::s_CompressHook(void *p)
{
    uint64_t cacheLimit = ((CompressParam *)p)->cacheLimit;
    char* fileName = ((CompressParam *)p)->fileName;
    int source;
    struct stat64 stbuf;
    gzFile gzFile;
    char *in = NULL;
    void *tmp = NULL;
    char gzFileName[512];
    uint64_t leftSize = 0;
    sprintf(gzFileName, "%s.gz",fileName);
    int flags = O_LARGEFILE;
    flags = flags | O_RDONLY;
    if(cacheLimit > 0)
        flags = flags | O_DIRECT;
    source = ::open(fileName, flags, 0644);
    if(source == -1)
    {
        fprintf(stderr, "Compress log file %s error: open file failed!\n", fileName);
        delete fileName;
        delete (CompressParam *)p;
        return NULL;
    }
    if (fstat64(source, &stbuf) != 0)
    {
        fprintf(stderr, "Compress log file %s error: access file info error!\n", fileName);
        ::close(source);
        delete fileName;
        delete (CompressParam *)p;
        return NULL;
    }
    leftSize = stbuf.st_size;
    gzFile = gzopen(gzFileName, "wb"); 
    if(!gzFile)
    {
        fprintf(stderr, "Compress log file %s error: open compressed file failed!\n", gzFileName); 
        ::close(source);
        delete fileName;
        delete (CompressParam *)p;
        return NULL;
    }
    if(cacheLimit > 0)
    {
        if(posix_memalign(&tmp, 512, CHUNK) != 0)
        {
            fprintf(stderr, "posix_memalignes error: %s\n", strerror(errno));
            ::close(source);
            delete fileName;
            delete (CompressParam *)p;
            return NULL;
        }
        in = static_cast<char *>(tmp);
    }
    else
    {
        tmp = (void *)malloc(CHUNK);
        if(tmp == NULL)
        {
            fprintf(stderr, "malloc mem error: %s\n", strerror(errno));
            ::close(source);
            delete fileName;
            delete (CompressParam *)p;
            return NULL;
        }
        in = static_cast<char *>(tmp);
    }
    while(leftSize > 0)
    {
        ssize_t readLen = ::read(source, in, CHUNK);
        if(readLen < 0)
        {
            fprintf(stderr, "Read log file %s error: %s\n", fileName, strerror(errno));
            break;
        }
        ssize_t writeLen = gzwrite(gzFile, in, readLen);
        if(readLen != writeLen)
        {
            fprintf(stderr, "Compress log file %s error: gzwrite error!\n", gzFileName);
            break;
        }
        if((ssize_t)leftSize < writeLen)
            leftSize = 0;
        else
            leftSize -= writeLen;
    }
    ::close(source);
    gzclose(gzFile);
    unlink(fileName);
    delete fileName;
    delete (CompressParam *)p;
    if(tmp)
        free(tmp); 
    if(cacheLimit > 0)
    {
	int fd = ::open(gzFileName, O_LARGEFILE | O_RDONLY, 0644);
        if(fd == -1)
        {
            fprintf(stderr, "Open %s error: %s!\n", gzFileName, strerror(errno));
        }
        else
        {
            if(posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED) != 0)
                fprintf(stderr, "Clear cache %s error: %s!\n", gzFileName, strerror(errno));
            ::close(fd);
        }
    }
    return NULL;
}


TULIP_END_NAMESPACE

