
#ifndef UTIL_OPTIONAL_H
#define UTIL_OPTIONAL_H

/**
 * @file optional.h
 * @date 18/12/2025
 * @author Julian Benzel
 * @brief type-generic optional value-container
 */

#include <assert.h>
#ifdef BUILD_TESTS
#include <stdio.h>
#endif

#define nullopt(type) (optional_t(type)){ .has_value = 0 }

#define optional_t(type) _##type##_optional_t

#define make_optional(type, val) (optional_t(type)){ .value = val, .has_value = 1 }

#define _decl_optional_struct(type) \
typedef struct { \
    bool has_value; \
    type value; \
} _##type##_optional_t

#define declare_optional(type) \
_decl_optional_struct(type)

#ifdef BUILD_TESTS
int optional_test(FILE* fno);
#endif

#endif