### Tulip log日志库简介

Tulip log是用C++语言实现的一个稳定高效的日志库。它不依赖与任何第三方库，主要应用于类Unix操作系统下。它具有以下优势：

1. **性能稳定，代码精炼，无任何第三方依赖，运行效率高；**
2. **兼顾一般常规的log配置文件选项，比如Java编写的log4j的配置格式，上手容易；**
3. **支持多线程同步快速输出日志；**
4. **支持多种日志输出方式，目前支持的有Console、File和Udp转发日志；**
5. **对最常用的输出日志到文件的方式，支持文件名滚动和较多丰富细节特性定义；**
6. **宏定义方式极大简化了日志接口使用，编写打印日志代码简要便利。**



### 使用方法

使用方法可参考examples/hello_tulip.cpp代码示例：

```
#include "tulip/TLogDefine.h"



int main(int argc, char** argv) {
    //configurate TLog from logger config file 'logger.conf'
    TLOG_CONFIG("logger.conf");

    //declare and setup tlog variable
    TLOG_DECLARE_AND_SETUP_LOGGER(HELLOTULIP, MAIN);

    for (uint32_t i = 0; i < 100; ++i) {
        //output log by convenient micro definitions
        TLOG_LOG(INFO,"the values is:[%u].", i);
        sleep(1);
    }

    //flush before shutdown TLog
    TLOG_LOG_FLUSH();

    //at last shutdown TLog
    TLOG_LOG_SHUTDOWN();

    return 0;
}

```

这里给出一个Tulip log的配置文件logger.conf内容可供参考：

```
#Tulip log配置文件

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
```



### 编译和安装

目前支持类Unix环境下编译安装，方法如下：

```
git clone https://github.com/apollo008/tulip.git tulip.git
cd tulip.git
mkdir build-dir
cd build-dir
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install  ../src
make -j10
make install
```

build完成后会在安装目标下生成bin、include、lib三个目录：

bin目录下hello_tulip可执行程序是tulip log演示程序；

lib目录下的libtulip.so是动态库，用以第三方项目使用；

include目录下是第三方项目需要include的头文件。



### 其它

相关细节或其它未尽事宜，可联系 dingbinthu@163.com 探讨咨询。

