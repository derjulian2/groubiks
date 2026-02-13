
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
    GROUBIKS_SHADER_ERROR,
    GROUBIKS_IO_ERROR,
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
read_file(const char* path, 
    char** ptr, 
    size_t* size
);

/**
 * @brief classic macro-overload counting-magic.
 */
#define __EXPAND_MACRO(x) x
#define __SELECT_MACRO(_1, _2, name, ...) name
/**
 * @brief    custom exception-handling utility. requires C23 for '__VA_OPT__'.
 *           (just some syntactic sugar to make error-handling less ugly)
 * @details  some explanation for my unnecessary but fancy macros:
 *
 * except -- defines a code-block that should only be executed in the
 *           case of an error. is jumped into by throw()-ing. control
 *           will resume just after the except-block.
 *
 * throw  -- will jump into the except-block in the current function.
 *           no-arg will do nothing but that, throw(<errvar>, <errval>)
 *           will also assign <errvar> = <errval>.
 */

#define except \
if (0) __except:

#define __throw_0 goto __except
#define __throw_1(_1) static_assert(0, "invalid number of arguments")
#define __throw_2(var, errval) { var = errval; __throw_0; }

#define throw(...) \
__EXPAND_MACRO( \
    __SELECT_MACRO( \
        __VA_OPT__(__VA_ARGS__,) \
        __throw_2, \
        __throw_1, \
        __throw_0  \
        ) __VA_OPT__((__VA_ARGS__)) \
)

#endif