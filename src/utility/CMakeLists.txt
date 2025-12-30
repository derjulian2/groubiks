
set(GROUBIKS_UTILITY_SOURCES
    "common.c"
    "log.c"
    "dynarray.c"
    "optional.c")

if (BUILD_VULKAN_RENDERER)
    target_sources(groubiks
        PUBLIC ${GROUBIKS_UTILITY_SOURCES})
endif()

if (BUILD_TESTS)
    add_executable(groubiks_utility_tests
        "main.c"
        ${GROUBIKS_UTILITY_SOURCES})

    target_compile_definitions(groubiks_utility_tests
        PUBLIC BUILD_TESTS)
        
    target_include_directories(groubiks_utility_tests
        PUBLIC ${GROUBIKS_INCLUDE_DIR})
endif()



