
#ifndef GROUBIKS_COMMON_H
#define GROUBIKS_COMMON_H

/**
 * @file common.h
 * @brief general types, functions and macros.
 */

#include <groubiks/utility/vector.h>

// enum Groubiks_ErrorCode
// {
//     SUCCESS,
//     OUT_OF_MEMORY,
//     VULKAN_ERROR
//     /* ... */
// };

typedef char* cstring_t;
typedef int GroubiksResult_t;

declare_vector(cstring_t)

#define memzero(obj) memset(&obj, 0, sizeof(obj))

#endif