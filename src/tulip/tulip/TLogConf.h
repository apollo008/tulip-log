/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogConf.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    It defines Tutlip TLog configuration file operator interfaces, such configurator log from some conf file.
**********************************************************************************/
#ifndef _TULIP_TLOG_CONF_H
#define _TULIP_TLOG_CONF_H

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "tulip/TLogger.h"
#include "tulip/TLogAppender.h"
#include "tulip/TLogConfigProps.h"
#include "tulip/TLogCommon.h"

using  namespace  std;

TULIP_BEGIN_NAMESPACE

/**
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:52
 *@brief     Configure file has format like this:
             tlog.rootLogger=INFO, tulipAppender,consoleAppender
             tlog.appender.tulipAppender=FileAppender
             tlog.appender.tulipAppender.max_file_size=3024
             tlog.appender.tulipAppender.fileName=logs/app.log
             tlog.appender.tulipAppender.flush=false
             tlog.appender.tulipAppender.delay_time=1
             tlog.appender.tulipAppender.layout=PatternLayout
             tlog.appender.tulipAppender.layout.LogPattern=[%%d] [%%t,%%F:%%n -- %%f() %%l] [%%m]

             tlog.appender.consoleAppender=ConsoleAppender
             tlog.appender.consoleAppender.delay_time=1
             tlog.appender.consoleAppender.layout=PatternLayout
             tlog.appender.consoleAppender.layout.LogPattern=[%%d] [%%t,%%F:%%n -- %%f() %%l] [%%m]


             tlog.appender.udpAppender=UdpAppender
             tlog.appender.udpAppender.ip=192.168.0.211
             tlog.appender.udpAppender.port=14878
             tlog.appender.udpAppender.layout=PatternLayout
             tlog.appender.udpAppender.layout.LogPattern=[%%d] [%%t,%%F:%%n -- %%f() %%l] [%%m]
             ####################################################################
*/

/**
 *@brief     TLog config file operator interface class 
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午9:53
 */
class TLogConf {
public:
    static bool s_ConfigureLogger(const char* loggerConfFile);
    static void s_ConfigureRootLogger();


    static bool s_IsConfigured() { return s_configured; }
    static void s_SetUnConfigured() { s_configured = false;}
private:
    static bool                  s_configured;
public:

private:
    static bool s_ConfigureLogger(const string& loggerName, map<string, TLogBaseAppender*> &allAppenders, TLogConfigProps &props);


    static void s_GlobalInit(TLogConfigProps &props);

    static void s_InitLoggers(vector<string>& loggers, TLogConfigProps &props);

    static void s_InitAllAppenders(map<string, TLogBaseAppender*>&allAppenders, TLogConfigProps &props);

    static TLogBaseAppender* s_InstantiateAppender(const string& name, TLogConfigProps &props);

    static void s_SetLayout(TLogBaseAppender* appender, const string& appenderName, TLogConfigProps &props);

    static uint32_t s_GetLevelByStr(const string &levelStr);
};

TULIP_END_NAMESPACE

#endif //_TULIP_TLOG_CONF_H
