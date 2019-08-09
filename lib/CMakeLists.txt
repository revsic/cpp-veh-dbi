cmake_minimum_required(VERSION 3.10)
project(libvehdbg)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")
include_directories(${CMAKE_SOURCE_DIR}/lib/include/impl)

add_library(libvehdbg STATIC ${SOURCES})
target_link_libraries(libvehdbg DbgHelp.lib)
