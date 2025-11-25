cmake_minimum_required(VERSION 4.0)
project(SparkerRenderer)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)

file(
        COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources"
)

file(
        MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data"
)

set(OUTPUT_DIR "\"${CMAKE_CURRENT_BINARY_DIR}\"")
set(RENDERER_RESOURCE_DIR "\"${CMAKE_CURRENT_BINARY_DIR}/resources\"")
set(RENDERER_DATA_DIR "\"${CMAKE_CURRENT_BINARY_DIR}/data\"")

configure_file(
        "SpRendererConfig.h.in"
        "SpRendererConfig.h"
        @ONLY
)


add_library(SparkerRenderer STATIC)

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
        "${CMAKE_CURRENT_BINARY_DIR}"
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(Vulkan REQUIRED)

target_link_libraries(SparkerRenderer Vulkan::Vulkan)
target_link_libraries(SparkerRenderer SDL3::SDL3)

target_compile_features(SparkerRenderer PUBLIC)