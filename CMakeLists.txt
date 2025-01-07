cmake_minimum_required(VERSION 3.29)
project(Raycaster)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/SDL)
add_subdirectory(src)

target_link_libraries(Raycaster PRIVATE SDL3::SDL3)
