cmake_minimum_required(VERSION 3.10)
project(readme_sample)

set(CMAKE_CXX_STANDARD 17)

add_library(readme_sample SHARED dllmain.cpp)
include_directories(${CMAKE_SOURCE_DIR}/../lib/include)

set(LIBRARY_DIRECTORY ${CMAKE_BINARY_DIR}/../lib/Release)
target_link_libraries(readme_sample Dbghelp.lib ${LIBRARY_DIRECTORY}/libvehdbg.lib)
