/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogUtil.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    TLog util interface definitions
**********************************************************************************/
#ifndef _TULIP_TLOG_UTIL_H
#define _TULIP_TLOG_UTIL_H

#include <string>
#include <vector>
#include <climits>
#include <stdarg.h>
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@brief     TLog utility class
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:34
 */
class TLogUtil {
public:
    static const char* s_WhiteSpace;
public:

    static uint32_t  s_StringSplit(vector<string>& result,
                                      const string& s,
                                      char delim,
                                      uint32_t maxSplitCnt = INT_MAX);

    static string s_StringTrim(const string& s);

    template<typename T>
    static uint32_t  s_StringSplit(T& result,
                                      const string& s,
                                      char delim,
                                      uint32_t maxSplitCnt = INT_MAX) {

        uint32_t t;
        string::size_type left = 0;

        for (t = 1; t < maxSplitCnt; t++) {
            string::size_type right = s.find(delim, left);
            if (string::npos == right) {
                break;
            }
            *result++ = s.substr(left, right - left);
            left = right + 1;
        }

        string lstSubStr = s.substr(left);

        if ( lstSubStr != "") {
            *result++ = lstSubStr;
        }

        return t;
    }

};


TULIP_END_NAMESPACE

#endif //_TULIP_TLOG_UTIL_H
