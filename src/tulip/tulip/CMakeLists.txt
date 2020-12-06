
add_subdirectory(examples)

add_library(tulip SHARED
        TLogItem.cpp
        TLogSynchronize.cpp
        TLogFormatter.cpp
        TLogLayout.cpp
        TLogItemQueue.cpp
        TLogUtil.cpp
        TLogConfigProps.cpp
        TLogAppender.cpp
        TLogThread.cpp
        TLogConf.cpp
        TLogger.cpp)

set_property(TARGET tulip PROPERTY CXX_STANDARD 11)

target_link_libraries(tulip z pthread)


install(TARGETS tulip
        DESTINATION lib)

install(FILES
        TLogAppender.h
        TLogCommon.h
        TLogConf.h
        TLogConfigProps.h
        TLogDefine.h
        TLogFormatter.h
        TLogger.h
        TLogItem.h
        TLogItemQueue.h
        TLogLayout.h
        TLogSynchronize.h
        TLogThread.h
        TLogUtil.h
        DESTINATION include/tulip)


