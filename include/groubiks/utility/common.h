
#ifndef GROUBIKS_COMMON_H
#define GROUBIKS_COMMON_H

/**
 * @file common.h
 * @brief general types, functions and macros.
 */

#include <stdint.h>
#include <stdio.h>
#include <groubiks/utility/dynarray.h>

/**
 * @brief error-codes.
 */
enum groubiks_error_code {
    GROUBIKS_SUCCESS,
    GROUBIKS_BAD_ALLOC,
    GROUBIKS_VULKAN_ERROR,
    GROUBIKS_GLFW_ERROR
};
typedef enum groubiks_error_code groubiks_result_t;
/**
 * @brief shorter integral types. 
 */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
/**
 * @brief common dynamic-array-types (integer, string).
 */
declare_dynarray(char*, str);
declare_dynarray(u32, u32);
/**
 * @brief some convenience/utility-macros and functions.
 */
#define memzero(obj) memset(&obj, 0, sizeof(obj))
#define clamp(v, lo, hi) _Generic (v, \
    int : clampi(v, lo, hi), \
    unsigned int : clampui(v, lo, hi) \
)

int 
clampi(int v, 
    int lo, 
    int hi
);

unsigned int 
clampui(unsigned int v, 
    unsigned int lo, 
    unsigned int hi
);

int 
readFile(const char* path, 
    char** ptr, 
    size_t* size
);

#endif