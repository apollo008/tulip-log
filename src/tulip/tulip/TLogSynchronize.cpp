/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       TLogSynchronize.h
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Implementation for some synchronize inteface used in Tulip about lock and conditions
**********************************************************************************/
#include "tulip/TLogSynchronize.h"

TULIP_BEGIN_NAMESPACE

TLogCondition::TLogCondition() {
    pthread_mutex_init(&m_lock, NULL);
    pthread_cond_init(&m_cond, NULL);
}

TLogCondition::~TLogCondition() {
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_lock);
}

void TLogCondition::Lock() {
    pthread_mutex_lock(&m_lock);
}
void TLogCondition::Unlock() {
    pthread_mutex_unlock(&m_lock);
}
void TLogCondition::Broadcast() {
    pthread_cond_broadcast(&m_cond);
}
void TLogCondition::Signal() {
    pthread_cond_signal(&m_cond);
}
void TLogCondition::Wait() {
    pthread_cond_wait(&m_cond, &m_lock);
}

TULIP_END_NAMESPACE

