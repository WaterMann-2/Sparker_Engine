cmake_minimum_required(VERSION 4.0)
project(Sparker_Engine)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include_directories(PUBLIC
        "./external/glfw-3.4/include/glfw"
        "./external/glm-master/glm"
        "./external/sdl3"
        "./external/Vulkan-SDK-1.4.328.1/x86_64/include"
)

#add_subdirectory("./external/sdl3"  EXCLUDE_FROM_ALL)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

find_package(Vulkan REQUIRED)

add_executable(Sparker_Engine main.cpp
        src/Renderer/RendererCore.cpp
        src/Renderer/RendererCore.h
        src/Renderer/Utils/Utils.cpp
        src/Renderer/Utils/Utils.h
        src/Renderer/QueueFamily.cpp
        src/Renderer/QueueFamily.h
)

target_link_libraries(Sparker_Engine Vulkan::Vulkan)
target_link_libraries(Sparker_Engine SDL3::SDL3)

