cmake_minimum_required(VERSION 4.0)
project(Sparker_Engine)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include_directories(PUBLIC
        "./external/glfw-3.4/include/glfw"
        "./external/glm-master/glm"
        "./external/sdl3"
        "./external/Vulkan-SDK-1.4.328.1/x86_64/include"
        "./external/shaderc/libshaderc"
)

#add_subdirectory("./external/sdl3"  EXCLUDE_FROM_ALL)

add_subdirectory(renderer)

add_executable(Sparker_Engine main.cpp)
target_link_libraries(Sparker_Engine PRIVATE SparkerRenderer)
