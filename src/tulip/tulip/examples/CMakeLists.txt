
add_executable(hello_tulip hello_tulip.cpp)
set_property(TARGET hello_tulip PROPERTY CXX_STANDARD 11)

target_link_libraries(hello_tulip tulip)


install(TARGETS hello_tulip
        DESTINATION bin)
