cmake_minimum_required(VERSION 3.17)
project(filter_ip)

set(CMAKE_CXX_STANDARD 20)

add_executable(filter_ip main.cpp)
target_compile_options(filter_ip PRIVATE -Werror)