cmake_minimum_required(VERSION 4.0)
project(SparkerRenderer)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)

add_library(SparkerRenderer STATIC
        include/SpRenderer/Utils.h)

target_sources(SparkerRenderer
    PRIVATE
        ${SP_RENDERER_SRCS}

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
        include/SpRenderer/QueueFamily.h
        include/SpRenderer/RendererCore.h
        include/SpRenderer/Shader.h
        include/SpRenderer/Utils.h
        include/SpRenderer/Vertex.h
)

target_include_directories(SparkerRenderer PRIVATE
        "./external/glfw-3.4/include/glfw"
        "./external/glm-master/glm"
        "./external/sdl3"
        "./external/Vulkan-SDK-1.4.328.1/x86_64/include"
        "./include/SpRenderer"
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(Vulkan REQUIRED)

target_link_libraries(SparkerRenderer Vulkan::Vulkan)
target_link_libraries(SparkerRenderer SDL3::SDL3)

target_compile_features(SparkerRenderer PUBLIC)