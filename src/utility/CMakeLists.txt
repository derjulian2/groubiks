
if (BUILD_TESTS)
    add_executable(groubiks_utility_tests
        "main.c"
        "string.c"
        "vector.c")

    target_compile_definitions(groubiks_utility_tests
        PUBLIC BUILD_TESTS)
        
    target_include_directories(groubiks_utility_tests
        PUBLIC ${GROUBIKS_INCLUDE_DIR})
endif()



