cmake_minimum_required(VERSION 4.0)
project(Sparker_Engine)

set(CMAKE_CXX_STANDARD 23)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include_directories(PUBLIC
        "$ENV{GLFW_INCLUDE_DIR}/include/GLFW"
        "$ENV{VULKAN_INCLUDE_DIR}/include"
        "$ENV{GLM_INCLUDE_DIR}/glm"
)

add_subdirectory("$ENV{SDL3_INCLUDE_DIR}" "$ENV{SDL3_INCLUDE_DIR}/build" EXCLUDE_FROM_ALL)
add_subdirectory("$ENV{GLFW_INCLUDE_DIR}" "$ENV{GLFW_INCLUDE_DIR}/build" EXCLUDE_FROM_ALL)

find_package(glfw3 3.4 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(Sparker_Engine main.cpp
        Renderer/RendererCore.cpp
        Renderer/RendererCore.h
        Renderer/Utils/utils.h
        Renderer/Utils/utils.cpp)

target_link_libraries(Sparker_Engine glfw)
target_link_libraries(Sparker_Engine SDL3::SDL3)
