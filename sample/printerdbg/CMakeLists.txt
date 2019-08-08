cmake_minimum_required(VERSION 3.10)
project(printerdbg)

set(CMAKE_CXX_STANDARD 17)

add_library(printerdbg SHARED dllmain.cpp custom_handler.cpp)
include_directories(${CMAKE_SOURCE_DIR}/../lib/include)

set(LIBRARY_DIRECTORY ${CMAKE_BINARY_DIR}/../lib/Release)
target_link_libraries(printerdbg Dbghelp.lib ${LIBRARY_DIRECTORY}/libvehdbg.lib)
