/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogDefine.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Some useful micro definitions for TLog use, such as TLOG_DECLARE,
  *                TLOG_SETUP, TLOG_LOG
**********************************************************************************/
#ifndef _TULIP_TLOG_DEFINE_H
#define _TULIP_TLOG_DEFINE_H


#include "tulip/TLogConf.h"
#include "tulip/TLogger.h"
#include <iostream>

#define TLOG_CONFIG_ROOT_LOGGER() tulip::TLogConf::s_ConfigureRootLogger()
#define TLOG_CONFIG(filename) do {                                           \
        try {                                                                \
            if(tulip::TLogConf::s_IsConfigured()) break;                     \
               if(!tulip::TLogConf::s_ConfigureLogger(filename)) exit(-1);   \
            }                                                                \
            catch(std::exception &e) {                                       \
                    std::cerr << "Error!!! Failed to configure logger"       \
                              << e.what() << std::endl;                      \
                    exit(-1);                                                \
           }                                                                 \
        }while(0)


#define TLOG_DECLARE() static tulip::TLogger *_logger

#define TLOG_SETUP(n, c) tulip::TLogger *c::_logger  \
    = tulip::TLogger::s_GetLogger(#n "." #c)

#define TLOG_SETUP_TEMPLATE(n, c, T) template <typename T> \
    tulip::TLogger *c<T>::_logger                          \
    = tulip::TLogger::s_GetLogger(#n "." #c)

#define TLOG_SET_ROOT_LOG_LEVEL(level)                                  \
    tulip::TLogger::s_GetRootLogger()->SetLevel(tulip::TLOG_LEVELNO_##level)

#define TLOG_DECLARE_AND_SETUP_LOGGER(n, c) static tulip::TLogger *_logger \
    = tulip::TLogger::s_GetLogger(#n "." #c)

#define TLOG_LOG_SHUTDOWN() tulip::TLogger::s_Shutdown()

#define TLOG_LOG_FLUSH() tulip::TLogger::s_FlushAll()

#define TLOG_LOGGER_LOG(logger, level, format, args...) {                      \
        if(__builtin_expect(logger->IsLevelEnabled(level), 0))          \
            logger->Log(level, __FILE__, __LINE__, __FUNCTION__, format, ##args);}

#define TLOG_LOG(level, format, args...) TLOG_LOGGER_LOG(_logger, tulip::TLOG_LEVELNO_##level, format, ##args)


#endif //_TULIP_TLOG_DEFINE_H
