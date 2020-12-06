/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogCommon.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Some macros definitions interface for common use
**********************************************************************************/
#ifndef _TULIP_TLOG_COMMON_H
#define _TULIP_TLOG_COMMON_H

#define TULIP_BEGIN_NAMESPACE namespace tulip {
#define TULIP_END_NAMESPACE }
#define TULIP_USE_NAMESPACE using namespace tulip


#define TULIP_NS tulip

#define TULIP_DELETE_AND_SET_NULL(x) do {        \
        if(x){                                  \
            delete x;                           \
            x = NULL;                           \
        }                                       \
    } while(0)

#define ARRAY_DELETE_AND_SET_NULL(x) delete [] x; x = NULL

#define likely(x) __builtin_expect((x), 1)

#define unlikely(x) __builtin_expect((x), 0)


#define COPY_CONSTRUCTOR(T) \
    T(const T &); \
    T & operator=(const T &);

#endif //_TULIP_TLOG_COMMON_H
