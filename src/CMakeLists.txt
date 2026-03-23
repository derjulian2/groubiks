
cmake_minimum_required(VERSION 3.30.0)

set(CMAKE_CXX_STANDARD 23)

project(neogroubiks)

set(NEOGROUBIKS_ROOT_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(NEOGROUBIKS_INCLUDE_DIR
    "../include")

cmake_path(ABSOLUTE_PATH NEOGROUBIKS_INCLUDE_DIR NORMALIZE)
cmake_path(GET NEOGROUBIKS_ROOT_DIR PARENT_PATH NEOGROUBIKS_ROOT_DIR)
cmake_path(ABSOLUTE_PATH NEOGROUBIKS_ROOT_DIR NORMALIZE)

set(NEOGROUBIKS_SOURCES
    "main.cpp"
    "Application.cpp"
    "glad.c"

    "Cube.cpp"
    "Camera.cpp"
    "Gl.cpp"
    "Vertex.cpp"
)

set(NEOGROUBIKS_DEPENDENCY_LIBS
    "sfml-system"
    "sfml-graphics"
    "sfml-window"
    "GL"
)

add_executable(neogroubiks 
    ${NEOGROUBIKS_SOURCES}
)

target_compile_definitions(neogroubiks PUBLIC
    NEOGROUBIKS_VERTEX_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/default.vert"
    NEOGROUBIKS_FRAGMENT_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/default.frag"    
)

add_subdirectory("imgui")

target_include_directories(neogroubiks
    PUBLIC ${NEOGROUBIKS_INCLUDE_DIR}
)

target_link_libraries(neogroubiks
    ${NEOGROUBIKS_DEPENDENCY_LIBS}
)