cmake_minimum_required(VERSION 3.30.0)

project(groubiks)

set(GROUBIKS_SOURCES
    main.c
    graphics.c)

set(GROUBIKS_INCLUDE_DIR
    "../include")

find_package(glfw3 REQUIRED)

add_executable(groubiks
    ${GROUBIKS_SOURCES})

target_include_directories(groubiks
    PUBLIC ${GROUBIKS_INCLUDE_DIR})

target_link_libraries(groubiks
    glfw
    vulkan
    dl
    pthread
    X11
    Xrandr
    Xxf86vm
    Xi)