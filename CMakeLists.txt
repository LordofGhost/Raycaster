cmake_minimum_required(VERSION 3.29)
project(Raycaster)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED) # This will find the OpenGL package on your system (installed by macOS)

add_subdirectory(lib/SDL)
add_subdirectory(src)

target_link_libraries(Raycaster PRIVATE OpenGL::GL SDL3::SDL3)
