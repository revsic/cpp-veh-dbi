cmake_minimum_required(VERSION 3.10)
project(branchtracer)

set(CMAKE_CXX_STANDARD 17)

add_executable(helper helper.cpp)

add_library(branchtracer SHARED dllmain.cpp)
include_directories(${CMAKE_SOURCE_DIR}/../lib/include)

set(LIBRARY_DIRECTORY ${CMAKE_BINARY_DIR}/../lib/Release)
target_link_libraries(branchtracer Dbghelp.lib ${LIBRARY_DIRECTORY}/libvehdbg.lib)
