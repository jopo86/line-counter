cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(line-counter)

add_executable(lc src/main.cpp)
install(TARGETS lc RUNTIME DESTINATION "C:/Program Files/line-counter/bin")
