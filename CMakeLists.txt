cmake_minimum_required(VERSION 3.29)
project(Raycaster LANGUAGES C CXX OBJC)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/SDL)
add_subdirectory(lib/SDL_image)
add_subdirectory(src)

# Copy the lib directory to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/res/textures DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(Raycaster PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
