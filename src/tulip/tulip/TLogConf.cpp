/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogConf.cpp
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementation for Tulip TLog config file operator inerface class
**********************************************************************************/
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <list>
#include "tulip/TLogConf.h"
#include "tulip/TLogUtil.h"

TULIP_BEGIN_NAMESPACE


/*static*/
bool TLogConf::s_configured = false;


/**
 *@brief     Configure logger from logger configiure file.
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:54
 */
bool TLogConf::s_ConfigureLogger(const char* loggerConfFileIn)
{
    if(s_configured){
        throw runtime_error(string("Log Config File ") + loggerConfFileIn + " has configured!!!");
    }
    s_configured = true;
    TLogger::s_Setup();

    ifstream ifs(loggerConfFileIn);
    if (!ifs) {
        throw runtime_error(string("Log Config File ") + loggerConfFileIn + " does not exist or is unreadable!");
    }

    TLogConfigProps props;
    props.Load(ifs);
    
    s_GlobalInit(props);

    map<string, TLogBaseAppender*> allAppenders;
    s_InitAllAppenders(allAppenders, props);

    vector<string> loggers;
    s_InitLoggers(loggers, props);

    for (vector<string>::const_iterator iter = loggers.begin(); iter != loggers.end(); ++iter) {
        if(!s_ConfigureLogger(*iter, allAppenders, props))
        {
            fprintf(stderr, "logger.conf format error, please check .\n");
            return false;
        }
    }
    return true;
}


/**
 *@brief     config root loggger 
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:54
 */
void TLogConf::s_ConfigureRootLogger()
{
    TLogger* root = TLogger::s_GetRootLogger();
    root->SetLevelNo(TLOG_LEVELNO_INFO);
    root->ResetAppender(TLogConsoleAppender::s_GetAppender());
}

/**
 *@brief     set max message length for global setting
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:58
 */
void TLogConf::s_GlobalInit(TLogConfigProps& props)
{
    int value = props.GetInt(string("max_msg_len"), 4096);
    if(value > 0)
        TLogger::s_MaxMsgLength = value;
}

/**
 *@brief     Initialize all TLog appenders
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:55
 */
void TLogConf::s_InitAllAppenders(map<string, TLogBaseAppender*> & appenders, TLogConfigProps &props)
{
    string curAppender;

    string prefix("appender");
    TLogConfigProps::const_iterator from = props.lower_bound(prefix + '.');
    TLogConfigProps::const_iterator to = props.lower_bound(prefix + (char)('.' + 1));
    for (TLogConfigProps::const_iterator it = from; it != to; ++it) {
        const string& key = (*it).first;
        list<string> propNameParts;
        back_insert_iterator<list<string> > pnpIt(propNameParts);
        TLogUtil::s_StringSplit(pnpIt, key, '.');
        list<string>::const_iterator it2  = propNameParts.begin();
        list<string>::const_iterator iEnd = propNameParts.end();
        if (++it2 == iEnd) {
            throw runtime_error(string("missing appender name"));
        }

        const string appenderName = *it2++;
        if (appenderName != curAppender) {
            if (it2 == iEnd) {
                curAppender = appenderName;
                appenders[curAppender] = s_InstantiateAppender(curAppender,props);
            }
            else {
                throw runtime_error(string("partial appender definition: ") + key);
            }
        }
    }
}

/**
 *@brief     configure logger by loggerName
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:58
 */
bool TLogConf::s_ConfigureLogger(const string&                    loggerName,
                                 map<string,TLogBaseAppender*>&   allAppenders,
                                 TLogConfigProps&                 props)
{
    string tmpLoggerName =
        (loggerName == "rootLogger") ? loggerName : "logger." + loggerName;

    TLogConfigProps::iterator iter = props.find(tmpLoggerName);
    if (props.end() == iter ) {
        if (tmpLoggerName == "rootLogger") {
            TLogger::s_GetRootLogger();
            return true;
        }
        throw runtime_error(string("Unable to find logger: ") + tmpLoggerName);
    }

    TLogger* logger = (loggerName == "rootLogger") ?
                     TLogger::s_GetRootLogger() : TLogger::s_GetLogger(loggerName.c_str());

    list<string> tokens;
    back_insert_iterator<list<string> > tokIt(tokens);
    TLogUtil::s_StringSplit(tokIt, (*iter).second, ',');
    list<string>::const_iterator t = tokens.begin();
    list<string>::const_iterator tEnd = tokens.end();

    uint32_t level = TLOG_LEVELNO_NOTSET;
    if (t != tEnd)
    {
        string  levelStr = TLogUtil::s_StringTrim(*t++);
        if (levelStr != "") {
            level = s_GetLevelByStr(levelStr);
            if (level >= TLOG_LEVELNO_NOTSET) {
                t--;
            }
        }
    }
    logger->SetLevelNo(level);

    bool inherit = props.GetBool("inherit." + loggerName, true);
    if (!logger->SetInherit(inherit)) {
        return false;
    }

    logger->ClearAppenders();
    for (; t != tEnd; ++t) {
        string appenderName = TLogUtil::s_StringTrim(*t);
        map<string, TLogBaseAppender*>::const_iterator appIt = allAppenders.find(appenderName);
        if (appIt == allAppenders.end()) {
            throw runtime_error(string("Appender '") + appenderName + "' not found for logger '" + loggerName + "'");
        }
        else {
            logger->AddAppender(((*appIt).second));
        }
    }
    return true;
}

/**
 *@brief     Instantiate one TLogAppender instance
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:58
 */
TLogBaseAppender* TLogConf::s_InstantiateAppender(const string& appenderName, TLogConfigProps &props)
{
    TLogBaseAppender* resultAppender = NULL;
    string appenderPrefixStr = string("appender.") + appenderName;

    TLogConfigProps::iterator it = props.find(appenderPrefixStr);
    if (props.end() == it)
        throw runtime_error(string("Appender '") + appenderName + "' not defined");

    string::size_type len = (*it).second.find_last_of(".");
    string appenderType = (len == string::npos) ? (*it).second : (*it).second.substr(len+1);
    if (appenderType == "ConsoleAppender") {
        resultAppender = TLogConsoleAppender::s_GetAppender();
        bool isFlush = props.GetBool(appenderPrefixStr + ".flush", false);
        resultAppender->SetAutoFlush(isFlush);
    }
    else if (appenderType == "UdpAppender") {
        string ip = props.GetString(appenderPrefixStr + ".ip", "127.0.0.1");
        int port = props.GetInt(appenderPrefixStr + ".port",8888);
        resultAppender = TLogUdpAppender::s_GetAppender(ip,port);
    }
    else if (appenderType == "FileAppender") {
        string fileName = props.GetString(appenderPrefixStr + ".fileName", "default.log");
        resultAppender = TLogFileAppender::s_GetAppender(fileName.c_str());
        bool isFlush = props.GetBool(appenderPrefixStr + ".flush", false);
        resultAppender->SetAutoFlush(isFlush);
        uint32_t delayHour = props.GetInt(appenderPrefixStr + ".delay_time", 0);
        resultAppender->SetDelayTime(delayHour);
        uint32_t cacheLimit = props.GetInt(appenderPrefixStr + ".cache_limit", 0);
        resultAppender->SetCacheLimit(cacheLimit);
        uint32_t maxFileSize = props.GetInt(appenderPrefixStr + ".max_file_size", 0);
        resultAppender->SetMaxSize(maxFileSize);
        bool isCompress = props.GetBool(appenderPrefixStr + ".compress", false);
        resultAppender->SetCompress(isCompress);
    }
    else {
        throw runtime_error(string("Appender '") + appenderName + "' has unknown type '" + appenderType + "'");
    }
    s_SetLayout(resultAppender, appenderName, props);

    return resultAppender;
}

void TLogConf::s_SetLayout(TLogBaseAppender* appender, const string& appenderName, TLogConfigProps &props)
{
    TLogConfigProps::iterator it  = props.find(string("appender.") + appenderName + ".layout");
    if (props.end() == it)
        return;
		
    string::size_type len  = (*it).second.find_last_of(".");
    string  lytType  = (len == string::npos) ? (*it).second : (*it).second.substr(len+1);
 
    TLogLayoutBase* layout;
    if ("BasicLayout" == lytType) {
        layout = new TLogBasicLayout();
    }
    else if ("SimpleLayout" == lytType) {
        layout = new TLogSimpleLayout();
    }
    else if ("PatternLayout" == lytType) {
        TLogPatternLayout* patternLayout = new TLogPatternLayout();
        it = props.find(string("appender.") + appenderName + ".layout.LogPattern");
        if (props.end() != it) {
            patternLayout->SetPattern((*it).second);
        }
        layout = patternLayout;
    }
    else {
        throw runtime_error(string("Unknown layout type '" +   lytType + "' for appender '") + appenderName + "'");
    }
    appender->SetLayout(layout);
}

void TLogConf::s_InitLoggers(vector<string>& loggers, TLogConfigProps &props) {
    loggers.clear();
    loggers.push_back(string("rootLogger"));
    string prefixstr("logger");
    TLogConfigProps::const_iterator from = props.lower_bound(prefixstr + '.');
    TLogConfigProps::const_iterator to = props.lower_bound(prefixstr + (char)('.' + 1));
    for (TLogConfigProps::const_iterator iter = from; iter != to; iter++) {
        loggers.push_back((*iter).first.substr(prefixstr.size() + 1));
    }

}

uint32_t TLogConf::s_GetLevelByStr(const string &levelStr)
{
    uint32_t levelNo;
    if (levelStr == "DISABLE")
        levelNo = TLOG_LEVELNO_DISABLE;
	else if (levelStr == "FATAL")
        levelNo = TLOG_LEVELNO_FATAL;
    else if (levelStr == "ERROR")
        levelNo = TLOG_LEVELNO_ERROR;
    else if (levelStr == "WARN")
        levelNo = TLOG_LEVELNO_WARN;
    else if (levelStr == "INFO")
        levelNo = TLOG_LEVELNO_INFO;
    else if (levelStr == "DEBUG")
        levelNo = TLOG_LEVELNO_DEBUG;
    else if (levelStr == "TRACE1")
        levelNo = TLOG_LEVELNO_TRACE1;
    else if (levelStr == "TRACE2")
        levelNo = TLOG_LEVELNO_TRACE2;
    else
        levelNo = TLOG_LEVELNO_NOTSET;

    return levelNo;
}





TULIP_END_NAMESPACE



