/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogItem.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Definitions for Tulip TLog Content items and operator interface
**********************************************************************************/
#ifndef _TULIP_TLOGITEM_H
#define _TULIP_TLOGITEM_H

#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include <memory>

#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@brief     Class for TLogger content data and operator interfaces
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:17
 */
class TLogItem {
public:

    TLogItem(const string&         loggerName,
             const string&         msg,
             const uint32_t             levelNo,
             const string&         file = "",
             const int&                 line = 0,
             const string&         func = "");


public:
    static const char* s_LevelName[];

private:
    static void s_GetCurrentTime(char cur[], int length);

public:
    string                      m_loggerName;
    char                             m_logTime[20]; //format: yyyy-mm-dd hh:mm:ss
    const string                m_levelStr;
    const uint32_t                   m_levelNo;  //level grade.
    const string                m_message;
    const string                m_file;
    const int                        m_line;
    const string                m_function;
    const int                        m_pid;
    const long                       m_tid;  //thread id

};
typedef shared_ptr<TLogItem> TLogItemPtr;


TULIP_END_NAMESPACE

#endif //_TULIP_TLOGITEM_H
