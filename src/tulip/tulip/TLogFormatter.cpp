/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogFormatter.cpp
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementations for Tulip TLog several formatter
**********************************************************************************/
#include "tulip/TLogFormatter.h"

TULIP_BEGIN_NAMESPACE

void TLogLevelFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item ->m_levelStr;
}

void TLogLevelFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_levelStr;
}

void TLogMessageFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item ->m_message;
}

void TLogMessageFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_message;
}

TLogStringFormatter::TLogStringFormatter(const std::string& str)
: m_str(str) {}

void TLogStringFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << m_str;
}

void TLogStringFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << m_str;
}



void TLogThreadIdFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_tid;
}

void TLogThreadIdFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_tid;
}

void TLogPIDFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_pid;
}

void TLogPIDFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_pid;
}


void TLogFuncFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_function;
}

void TLogFuncFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_function;
}

void TLogLineFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_line;
}

void TLogLineFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_line;
}


void TLogFileFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_file;
}

void TLogFileFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_file;
}


void TLogDateFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_logTime;
}

void TLogDateFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_logTime;
}




void TLogLogNameFormatter::Pushback(ostringstream& out, const TLogItemPtr  item) {
    out << item -> m_loggerName;
}

void TLogLogNameFormatter::Pushback(ostringstream& out, const TLogItem& item) {
    out << item.m_loggerName;
}









TULIP_END_NAMESPACE

