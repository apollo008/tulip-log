/********************************************************************************* 
  *Copyright(C),dingbinthu@163.com
  *All rights reserved. 
  *
  *FileName:       TLogFormatter.h
  *Author:         dingbinthu@163.com    
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    formatter interfaces for Tulip TLog 
**********************************************************************************/
#ifndef _TULIP_TLOGFORMATTER_H
#define _TULIP_TLOGFORMATTER_H

#include <sstream>
#include "tulip/TLogItem.h"
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@brief     Base formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:04
 */
class TLogBaseFormatter
{
public:
    inline virtual ~TLogBaseFormatter() {};
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item) = 0;
    virtual void Pushback(ostringstream& out, const TLogItem& item) = 0;
};


/**
 *@brief     Log level formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:05
 */
class TLogLevelFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};


/**
 *@brief     message formatter classs
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:05
 */
class TLogMessageFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};

/**
 *@brief     Class for string formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:05
 */
class TLogStringFormatter : public TLogBaseFormatter
{
public:
    TLogStringFormatter(const string& literal);

    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
private:
    string m_str;
};


/**
 *@brief     class for thread id formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:05
 */
class TLogThreadIdFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};


/**
 *@brief     class for pid formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:05
 */
class TLogPIDFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};


/**
 *@brief     class for function name formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:06
 */
class TLogFuncFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};

/**
 *@brief     class for line number TLog formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:06
 */
class TLogLineFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};


/**
 *@brief     class for filename formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:06
 */
class TLogFileFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};


/**
 *@brief     class for date formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:06
 */
class TLogDateFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};

/**
 *@brief     class for logger name formatter
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:07
 */
class TLogLogNameFormatter : public TLogBaseFormatter
{
public:
    virtual void Pushback(ostringstream& out, const TLogItemPtr  item);
    virtual void Pushback(ostringstream& out, const TLogItem& item);
};



TULIP_END_NAMESPACE

#endif //_TULIP_TLOGFORMATTER_H
