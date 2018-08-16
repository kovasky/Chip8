cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "-Wall")

add_executable(chip8 src/Chip8.cpp src/main.cpp)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

target_include_directories(chip8 PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8 ${SDL2_LIBRARIES})