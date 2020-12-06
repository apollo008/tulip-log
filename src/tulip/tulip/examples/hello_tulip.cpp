/*********************************************************************************
  *Copyright(C),dingbinthu@163.com
  *All rights reserved.
  *
  *FileName:       hello_tulip.cpp
  *Author:         dingbinthu@163.com
  *Version:        1.0
  *Date:           2020/12/6
  *Description:    Demo how to use Tulip TLog
**********************************************************************************/
#include "tulip/TLogDefine.h"

/**
 *@brief     Demo class, Demo how to log with class method
 *@author    dingbinthu@163.com
 *@date      2020/12/6, 上午10:38
 */
class Demo {
public:
    Demo() { }
    ~Demo() { }

public:
    void Run();
private:
    int  val = 100;
private:
    TLOG_DECLARE();
};

/**
 *@brief     declare tlog variable
 */
TLOG_SETUP(hello_tulip,Demo);

void Demo::Run() {
    while (true) {
        ++val;
        TLOG_LOG(WARN,"now value of val is :[%d].", val);
        sleep(2);
    }
}


static void* demo_hook(void *arg)
{
    Demo* d = (Demo*)arg;
    d->Run();
    return (void*)0;
}


int main(int argc, char** argv) {

    // configurate TLog from logger config file 'logger.conf'
    TLOG_CONFIG("logger.conf");

    //declare and setup tlog variable
    TLOG_DECLARE_AND_SETUP_LOGGER(HELLOTULIP, MAIN);

    pthread_t tid;
    pthread_create(&tid, NULL, demo_hook, new Demo());


    for (uint32_t i = 0; i < 100; ++i) {
        //output log by convenient micro definitions
        TLOG_LOG(INFO,"the values is:[%u].", i);
        sleep(1);
    }

    //flush before shutdown TLog
    TLOG_LOG_FLUSH();

    //at last, do not forget shutdown TLog
    TLOG_LOG_SHUTDOWN();

    return 0;
}