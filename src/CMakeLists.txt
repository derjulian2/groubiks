cmake_minimum_required(VERSION 3.30.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

project(groubiks)

find_package(glfw3 REQUIRED)

option(BUILD_VULKAN_RENDERER OFF)
option(BUILD_TESTS OFF)

set(BUILD_VULKAN_RENDERER ON)
set(BUILD_TESTS OFF)

set(GROUBIKS_SOURCES
    "main.c"
    "Application.c")

set(GROUBIKS_ROOT_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(GROUBIKS_INCLUDE_DIR
    "../include")

cmake_path(ABSOLUTE_PATH GROUBIKS_INCLUDE_DIR NORMALIZE)
cmake_path(GET GROUBIKS_ROOT_DIR PARENT_PATH GROUBIKS_ROOT_DIR)
cmake_path(ABSOLUTE_PATH GROUBIKS_ROOT_DIR NORMALIZE)

set(GROUBIKS_LIBRARY_DEPENDENCIES
    glfw
    vulkan
    dl
    pthread
    X11
    Xrandr
    Xxf86vm
    Xi)

if (BUILD_VULKAN_RENDERER)

    add_executable(groubiks
        ${GROUBIKS_SOURCES})

    add_subdirectory("renderer")

    target_compile_definitions(groubiks
        PUBLIC GLFW_INCLUDE_VULKAN)

    target_include_directories(groubiks
        PUBLIC ${GROUBIKS_INCLUDE_DIR})

    target_link_libraries(groubiks
        PUBLIC ${GROUBIKS_LIBRARY_DEPENDENCIES})

endif()

add_subdirectory("utility")