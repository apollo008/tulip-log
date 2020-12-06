/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogConfigProps.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Definition for Tulip TLog configuration file content properties
**********************************************************************************/
#ifndef _TULIP_TLOG_CONFIG_PROPS_H
#define _TULIP_TLOG_CONFIG_PROPS_H

#include <string>
#include <iostream>
#include <map>
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE


/**
 *@brief     Class for define properties from TLog config file content 
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:01
 */
class TLogConfigProps : public map<string,string> {
public:
    TLogConfigProps() {}
    ~TLogConfigProps() { }

    void      Load(istream& in);

    bool      GetBool(const string& prop, bool defValue);
    int       GetInt(const string& prop, int defValue);
    string    GetString(const string& prop, const char* defValue);

private:
    void ReplaceVars(string& value);
};


TULIP_END_NAMESPACE

#endif //_TULIP_TLOG_CONFIG_PROPS_H
