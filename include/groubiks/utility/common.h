
#ifndef GROUBIKS_COMMON_H
#define GROUBIKS_COMMON_H

/**
 * @file common.h
 * @brief general types, functions and macros.
 */

#include <stdint.h>
#include <stdio.h>
#include <groubiks/utility/vector.h>

// enum Groubiks_ErrorCode
// {
//     SUCCESS,
//     OUT_OF_MEMORY,
//     VULKAN_ERROR
//     /* ... */
// };

#define GROUBIKS_SUCCESS  0
#define GROUBIKS_ERROR   -1

typedef char* cstring_t;
typedef const char* const_cstring_t;
typedef int groubiks_result_t;

declare_vector(cstring_t);
declare_vector(const_cstring_t);
declare_vector(uint32_t);

define_vector_default_compare_predicate(uint32_t);

#define memzero(obj) memset(&obj, 0, sizeof(obj))

#define clamp(v, lo, hi) _Generic (v, \
    int : clampi(v, lo, hi), \
    unsigned int : clampui(v, lo, hi) \
)

int clampi(int v, int lo, int hi);
unsigned int clampui(unsigned int v, unsigned int lo, unsigned int hi);

int readFile(const char* path, char** ptr, size_t* size);

#endif