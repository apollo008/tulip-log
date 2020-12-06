/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogLayout.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementation for Tulip TLog layout class
**********************************************************************************/
#include "tulip/TLogLayout.h"
#include <sstream>

TULIP_BEGIN_NAMESPACE


/*static */
string TLogPatternLayout::s_DefaultPattern = "%%d %%l %%c (%%p/%%t) : %%m";

TLogPatternLayout::TLogPatternLayout() {
    SetPattern( TLogPatternLayout::s_DefaultPattern );
}

TLogPatternLayout::~TLogPatternLayout() {
    ClearPattern();
}


string TLogPatternLayout::Place(const TLogItemPtr item) {
    if (nullptr != item) {
        ostringstream msg;
        for (size_t t = 0; t < m_formatters.size(); ++t) {
            m_formatters[t] -> Pushback(msg, item);
        }
        msg << endl;
        return msg.str();
    }
    else {
        return "";
    }
}

string TLogPatternLayout::Place(const TLogItem& item) {
    ostringstream msg;
    for (size_t t = 0; t < m_formatters.size(); ++t) {
        m_formatters[t] -> Pushback(msg, item);
    }
    msg << endl;
    return msg.str();
}


void TLogPatternLayout::ClearPattern() {
    m_pattern.clear();
    for (size_t i = 0; i < m_formatters.size(); ++i) {
        delete m_formatters[i];
    }
    m_formatters.clear();
}




string
TLogBasicLayout::Place(const TLogItemPtr item) {
    if(nullptr != item) {
        ostringstream str;

        str << item->m_logTime << " " << item->m_levelStr << " "
            << item->m_loggerName << " : " << item->m_message << endl;

        return str.str();
    }
    else {
        return "";
    }
}

string TLogBasicLayout::Place(const TLogItem& item) {
    ostringstream message;
    message << item.m_logTime << " " << item.m_levelStr << " "
            << item.m_loggerName << " : " << item.m_message << endl;
    return message.str();
}




string TLogSimpleLayout::Place(const TLogItemPtr item)
{
    if (nullptr != item) {
        ostringstream msg;

        msg << item->m_logTime << " " << item->m_levelStr << " "
            << item->m_loggerName<< " ("<< item->m_pid << "/" << item->m_tid<<") : "
            << item->m_message << endl;

        return msg.str();
    }
    else {
        return "";
    }
}


string TLogSimpleLayout::Place(const TLogItem& item)
{
    ostringstream msg;

    msg << item.m_logTime << " " << item.m_levelStr << " "
        << item.m_loggerName << " ("<< item.m_pid << "/" << item.m_tid <<") : "
        << item.m_message << endl;

    return msg.str();
}




void TLogPatternLayout::SetPattern(const string &pattern) {
    ClearPattern();

    TLogBaseFormatter* formatter = NULL;
    string str;
    for(string::const_iterator it = pattern.begin(); it != pattern.end(); it++) {
        if(*it == '%' && it + 2 != pattern.end()) {
            char ch = *(++it);
            if(ch == '%') {
                char nextCh = *(++it);
                switch(nextCh) {
                    case 'p': formatter = new TLogPIDFormatter(); break;
                    case 't': formatter = new TLogThreadIdFormatter(); break;
                    case 'l': formatter = new TLogLevelFormatter(); break;
                    case 'd': formatter = new TLogDateFormatter(); break;
                    case 'F': formatter = new TLogFileFormatter(); break;
                    case 'f': formatter = new TLogFuncFormatter(); break;
                    case 'n': formatter = new TLogLineFormatter(); break;
                    case 'm': formatter = new TLogMessageFormatter(); break;
                    case 'c': formatter = new TLogLogNameFormatter(); break;
                    default: str += "%%"; str += nextCh;
                }

                if(NULL != formatter) {
                    if (!str.empty()) {
                        m_formatters .push_back(new TLogStringFormatter (str));
                        str = "";
                    }
                    m_formatters .push_back(formatter);
                    formatter = NULL;
                }
            }
            else {
                str += '%';
                str += ch;
            }
        }
        else {
            str += *it;
        }
    }
    if (!str .empty()) {
        m_formatters .push_back(new TLogStringFormatter (str));
    }
    m_pattern = pattern;
}




TULIP_END_NAMESPACE

