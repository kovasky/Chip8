cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "-Wall")

add_executable(chip8 Chip8.cpp main.cpp)
