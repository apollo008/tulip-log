/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogLayout.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Definitions for Tulip TLog layout class
**********************************************************************************/
#ifndef _TULIP_TLOGLAYOUT_H
#define _TULIP_TLOGLAYOUT_H


#include <memory>
#include <vector>

#include <string>
#include <tulip/TLogItem.h>
#include <tulip/TLogFormatter.h>
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@brief     Base layout class
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:23
 */
class TLogLayoutBase {
public:
    virtual ~TLogLayoutBase() { }
    virtual string Place(const TLogItemPtr  item) = 0;
    virtual string Place(const TLogItem&    item) = 0;
};

/**
 *@brief     Basic layout class
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:23
 */
class TLogBasicLayout : public TLogLayoutBase
{
public:
    virtual ~TLogBasicLayout() { }
    virtual string Place(const TLogItem& event);
    virtual string Place(const TLogItemPtr event);
};

/**
 *@brief     simple layout class
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:23
 */
class TLogSimpleLayout : public TLogLayoutBase
{
public:
    virtual ~TLogSimpleLayout() { }
    virtual string Place(const TLogItem& event);
    virtual string Place(const TLogItemPtr event);
};

/**
 *@brief     Pattern layout class, often used
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:24
 */
class TLogPatternLayout : public TLogLayoutBase
{
public:
    TLogPatternLayout();
    virtual ~TLogPatternLayout();

    virtual string Place(const TLogItem& event);
    virtual string Place(const TLogItemPtr event);


    virtual void ClearPattern();
    /**
      * @brief set pattern format as follows, which are used in printf()
      * @n %%c - the logger name
      * @n %%l - the level
      * @n %%d - the date
      * @n %%m - the message
      * @n %%p - the process id
      * @n %%t - the thread id
      * @n %%n - the line number
      * @n %%F - the file name
      * @n %%f - the function name
      * @param logPattern the conversion pattern
      **/
    virtual void SetPattern(const string& pattern);

public:
    static string s_DefaultPattern;
private:
    string                               m_pattern;
    vector<TLogBaseFormatter*>           m_formatters;
};




TULIP_END_NAMESPACE

#endif //_TULIP_TLOGLAYOUT_H
