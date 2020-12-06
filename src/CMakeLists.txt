cmake_minimum_required(VERSION 3.5)

project(tulip)

set(TULIP_ALL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TULIP_ALL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}) 

set(ENV{CC} gcc)
set(ENV{CXX} g++)
message("PATH=$ENV{PATH},CC=$(CC),CXX=$(CXX)")
message("LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}")

add_definitions(-Wall -fPIC -m64 -D_FILE_OFFSET_BITS=64 -std=c++17 -gdwarf-3 -msse4.2)
set (CMAKE_SHARED_LINKER_FLAGS "-rdynamic")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

include_directories(
		${TULIP_ALL_SOURCE_DIR}/tulip
		)

add_subdirectory(tulip)

