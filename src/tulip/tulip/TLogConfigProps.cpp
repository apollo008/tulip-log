/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogConfigProps.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementation for Tulip TLog configuration file content properties interface
**********************************************************************************/
#include "tulip/TLogConfigProps.h"
#include "tulip/TLogUtil.h"

TULIP_BEGIN_NAMESPACE


/**
 *@brief     substitute ${xx} variables in string literal
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:01
 */
void TLogConfigProps::ReplaceVars(string& val) {
    string result;

    string::size_type leftPos = 0;
    string::size_type rightPos = val.find("${", leftPos);
    if (string::npos == rightPos ) {
        return;
    }

    while (true) {
        result += val.substr(leftPos, rightPos - leftPos);
        if (rightPos == string::npos) {
            break;
        }

        leftPos = rightPos + 2;
        rightPos = val.find('}', leftPos);
        if (string::npos == rightPos ) {
            result += val.substr(leftPos - 2);
            break;
        }
        else {
            const string key = val.substr(leftPos, rightPos - leftPos);
            if (key == "${") {
                result += "${";
            }
            else {
                char* v = getenv(key.c_str());
                if (v ) {
                    result += v;
                }
                else {
                    const_iterator it = find(key);
                    if (it != end()) {
                        result += it-> second;
                    }
                }
            }
            leftPos = rightPos + 1;
        }

        rightPos = val.find("${", leftPos);
    }

    val = result;
}


/**
 *@brief     Load properties from TLog config file stream
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:02
 */
void TLogConfigProps::Load(istream& in) {
    clear();

    string wholeLine, cmd;
    string lhs, rhs;
    char curLine[256];
    string::size_type len;

    while (in.getline(curLine, 256)) {
        wholeLine = curLine;

        len = wholeLine.find('#');
        if (len == string::npos) {
            cmd = wholeLine;
        }
        else if (len > 0) {
            cmd = wholeLine.substr(0, len);
        }
        else {
            continue;
        }

        len = cmd.find('=');
        if (string::npos != len) {
            rhs = TLogUtil::s_StringTrim(cmd.substr(len + 1, cmd.size() - len));
            lhs = TLogUtil::s_StringTrim(cmd.substr(0, len));
            ReplaceVars(rhs);
        }
        else {
            continue;
        }

        len = lhs.find('.');
        if (lhs.substr(0, len) == "tlog")
            lhs = lhs.substr(len + 1);

        insert(value_type(lhs, rhs));
    }
}

/**
 *@brief     Get integer value for property
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:02
 */
int TLogConfigProps::GetInt(const string& prop, int defValue)  {
    const_iterator it = find(prop);
    return (it == end()) ? defValue : atoi(it->second.c_str());
}

/**
 *@brief     Get bool value for property
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:02
 */
bool TLogConfigProps::GetBool(const string& prop, bool defValue)
{
    const_iterator it = find(prop);
    return (it == end()) ? defValue : (it->second == "true");
}

/**
 *@brief     Get string value for property
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:02
 */
string TLogConfigProps::GetString(const string& prop, const char* defValue)
{
    const_iterator it = find(prop);
    return (it == end()) ? string(defValue) : it->second;
}





TULIP_END_NAMESPACE
