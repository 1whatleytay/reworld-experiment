cmake_minimum_required(VERSION 3.15)
project(reworld)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
add_subdirectory(glfw)
add_library(lodepng lodepng/lodepng.h lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC lodepng)

add_subdirectory(engine)
add_subdirectory(world)

add_executable(game main.cpp)
target_link_libraries(game PRIVATE world)

# Copy Shaders
add_custom_command(TARGET game
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/main.frag ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/main.vert ${CMAKE_CURRENT_BINARY_DIR})
