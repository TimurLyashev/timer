cmake_minimum_required(VERSION 3.5)

project(timer_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(timer_test main.cpp)

target_include_directories(timer_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
