
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

/**
 * @brief classic macro-overload counting-magic.
 */
#define _EXPAND_MACRO(x) x
#define _SELECT_MACRO(_1, _2, name, ...) name
/**
 * @brief custom exception-handling utility.
 *        (just some syntactic sugar to make error-handling less ugly)
 * @details some explanation for my unnecessary but fancy macros:
 *
 * check(cond) will jump to the except-block if cond evaluates to false.
 * check(cond, statement) will execute the statement and then jump 
 *                        to the except-block if cond evaluates to false.
 * except(retval, block) will define an except-block that will execute when an exception
 *                       occured and then immediately return retval.
 * except(block)  will define an except-block, that will execute when an exception occured and
 *                jump to the defined cleanup-block.
 * cleanup(block) will define a cleanup-block, that will execute after the except-block
 *                and also if no exceptions occur.
 */
#define __check_0(cond) \
if (!(cond)) { \
    goto __except; \
} 

#define __check_1(cond, block) \
if (!(cond)) { \
    block; \
    goto __except; \
} 

#define check(...) _EXPAND_MACRO(_SELECT_MACRO(__VA_ARGS__, __check_1, __check_0)(__VA_ARGS__))

#define __except_0(block) \
__except: \
    block; \
    goto __cleanup;

#define __except_1(retval, block) \
__except: \
    block; \
    return retval;

#define except(...) _EXPAND_MACRO(_SELECT_MACRO(__VA_ARGS__, __except_1, __except_0)(__VA_ARGS__))

#define cleanup(block) \
__cleanup: \
    block;

#endif