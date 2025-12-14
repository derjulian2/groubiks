cmake_minimum_required(VERSION 3.30.0)

project(groubiks)

set(GROUBIKS_SOURCES
    main.c)

add_executable(groubiks
    ${GROUBIKS_SOURCES})