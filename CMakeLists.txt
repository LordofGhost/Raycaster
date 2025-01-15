cmake_minimum_required(VERSION 3.29)
project(Raycaster)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED) # This will find the OpenGL package on your system (installed by macOS)
add_subdirectory(lib/glfw) # This will add the glfw target (installed via git submodule)

# GLEW (installed via homebrew)
set(GLEW_INCLUDE_DIRS /opt/homebrew/Cellar/glew/2.2.0_1/include)
set(GLEW_LIBRARIES /opt/homebrew/lib/libGLEW.a)
include_directories(${GLEW_INCLUDE_DIRS})

add_subdirectory(lib/SDL)
add_subdirectory(src)

target_link_libraries(Raycaster PRIVATE OpenGL::GL glfw ${GLEW_LIBRARIES} SDL3::SDL3)
