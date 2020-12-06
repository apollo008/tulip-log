/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogUtil.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    TLog util interface implementations
**********************************************************************************/
#include "tulip/TLogUtil.h"

TULIP_BEGIN_NAMESPACE

/*static */
const char* TLogUtil::s_WhiteSpace = " \t\r\n";

/*static */
uint32_t  TLogUtil::s_StringSplit(vector<string>& result,
                                  const string& s,
                                  char delim,
                                  uint32_t maxSplitCnt /*= INT_MAX*/) {
    result.clear();
    back_insert_iterator<vector<string> > it(result);
    return TLogUtil::s_StringSplit(it, s, delim, maxSplitCnt );
}

/*static */
string TLogUtil::s_StringTrim(const string& s) {
    if (s.empty()) {
        return s;
    }
    string::size_type b = s.find_first_not_of(TLogUtil::s_WhiteSpace);
    if (string::npos == b) {
        return "";
    }
    string::size_type e = s.find_last_not_of(TLogUtil::s_WhiteSpace);
    return string(s, b, e - b + 1);
}


TULIP_END_NAMESPACE
