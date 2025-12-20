
#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

/**
 * @file vector.h
 * @date 17/12/2025
 * @author Julian Benzel
 * @brief type-generic dynamically-sized array data-structure. 
 *        usage:
 *        your_file.h : declare_vector(<type>)
 *        your_file.c : define_vector(<type>)
 *
 *        vector_t(<type>) my_vec = make_vector(<type>, ...)
 * @todo  add .capacity member to implement reserving space to not have to realloc at every insert/delete.
 */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#ifdef BUILD_TESTS
#include <stdio.h>
#endif
/**
 * @brief invalid vector-instance.
 */
#define null_vector(type) (vector_t(type)){ NULL, 0ll }
typedef int vector_result_t;
typedef size_t vector_index_t;
/**
 * @brief type-generic vector and iterator type.
 */
#define vector_t(type) _##type##_vector_t
#define vector_iterator_t(type) _##type##_vector_iterator_t
#define vector_comparison_fn_t(type) _##type##_vector_comp_fn_t
/**
 * @brief creates a vector of size `num` and can optionally initialize with the data stored in `data`.
 */
#define make_vector(type, data, num, err) _##type##_make_vector(data, num, err)
#define free_vector(vec) free((vec)->data)
/**
 * @brief accessors. begin() at &data[0], end() at &data[size]
 */
#define vector_begin(type, vec) ((vec)->data)
#define vector_end(type, vec) ((vec)->data + (vec)->size)
#define vector_at(type, vec, idx) ((vec)->data + idx)
/**
 * @brief inserts new elements before the element at `idx`.
 */
#define vector_insert(type, vec, idx, val, err) _##type##_vector_insert_value(vec, idx, val, err)
#define vector_insert_range(type, vec, idx, src, num, err) _##type##_vector_insert(vec, idx, src, num, err)
#define vector_push_back(type, vec, val, err) vector_insert(type, vec, (vec)->size, val, err)
/**
 * @brief erases elements at `idx`.
 */
#define vector_erase_range(type, vec, idx, num, err) _##type##_vector_erase(vec, idx, num, err)
#define vector_erase(type, vec, idx, err) vector_erase_range(type, vec, idx, 1, err)
/**
 * @brief zeroes all elements in the vector.
 */
#define vector_zero(type, vec) memset((vec)->data, 0, sizeof(type) * (vec)->size)
/**
 * @brief requires `define_vector_default_compare_predicate(<type>)`
 */
#define vector_default_compare_predicate(type) &_##type##_vector_default_compare_predicate
/**
 * @returns true if vector contains element of value `val`
 */
#define vector_contains(type, vec, val) _##type##_vector_contains_pred(vec, val, vector_default_compare_predicate(type))
#define vector_contains_predicate(type, vec, val, pred) _##type##_vector_contains_pred(vec, val, pred)
/**
 * @returns iterator to the first element of value `val`
 */
#define vector_find(type, vec, val) _##type##_vector_find_pred(vec, val, vector_default_compare_predicate(type))
#define vector_find_predicate(type, vec, val, pred) _##type##_vector_find_pred(vec, val, pred)
/**
 * @brief produces an index from an iterator and a vector.
 */
#define vector_make_index(type, vec, iter) ((ptrdiff_t)(iter - (vec)->data))
/**
 * @brief convenience macro for vector-traversal.
 */
#define vector_for_each(type, vec, name)  \
for (vector_iterator_t(type) name = vector_begin(type, vec); \
    name != vector_end(type, vec);  \
    ++name)

/**
 * IMPLEMENTATION
 */
#define _decl_vector_struct(type) \
typedef struct { \
    type* data; \
    size_t size; \
} _##type##_vector_t; \
typedef type* _##type##_vector_iterator_t; \
typedef bool(*_##type##_vector_comp_fn_t)(type, type);

#define _decl_vector_functions(type) \
vector_t(type) _##type##_make_vector(type* data, size_t num, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_insert(vector_t(type)* vec, vector_index_t idx, type* src, size_t num, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_insert_value(vector_t(type)* vec, vector_index_t idx, type val, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_erase(vector_t(type)* vec, vector_index_t idx, size_t num, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_find_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred); \
bool _##type##_vector_contains_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred);

/**
 * @brief declares a vector with `type` as it's value-type.
 *        use this in your header-files.
 */
#define declare_vector(type) \
_decl_vector_struct(type) \
_decl_vector_functions(type)

#define _def_make_vector(type) \
vector_t(type) \
_##type##_make_vector(type* data, size_t num, vector_result_t* err) { \
    vector_result_t res = 0; \
    vector_t(type) tmp = { \
        .data = malloc(sizeof(type) * num), \
        .size = num \
    }; \
    res = tmp.data == NULL; \
    if (err) \
    { *err = res; } \
    if (res) \
    { return null_vector(type); } \
    if (data) \
    { memcpy(tmp.data, data, sizeof(type) * num); } \
    return tmp; \
}

#define _def_vector_insert_range(type) \
vector_iterator_t(type) \
_##type##_vector_insert(vector_t(type)* vec, \
        vector_index_t idx, \
        type* src, \
        size_t num, \
        vector_result_t* err) { \
    assert(vec != NULL); \
    vector_result_t res = 0; \
    type* ptr = realloc(vec->data, sizeof(type) * (vec->size + num)); \
    if (err) \
    { *err = res; } \
    if (res) \
    { return NULL; } \
    /* move all old elements after the to-be-inserted-range over */ \
    memmove(ptr + (idx + num), \
            ptr + idx, \
            sizeof(type) * (vec->size - idx)); \
    /* copy the to-be-inserted-range over */ \
    memcpy(ptr + idx, \
           src, \
           sizeof(type) * num); \
    /* update */ \
    vec->data = ptr; \
    vec->size = vec->size + num; \
    return vec->data + idx; \
}

#define _def_vector_insert_value(type) \
vector_iterator_t(type) \
_##type##_vector_insert_value(vector_t(type)* vec, \
        vector_index_t idx, \
        type val, \
        vector_result_t* err) { \
    return vector_insert_range(type, vec, idx, &val, 1, err); \
}

#define _def_vector_erase_range(type) \
vector_iterator_t(type) \
_##type##_vector_erase(vector_t(type)* vec, \
        vector_index_t idx, \
        size_t num, \
        vector_result_t* err) { \
    assert(vec != NULL); \
    assert(num <= vec->size - idx); \
    vector_result_t res = 0; \
    type* ptr = malloc(sizeof(type) * (vec->size - num)); \
    if (err) \
    { *err = res; } \
    if (res) \
    { return NULL; } \
    /* copy all elements until the to-be-deleted-range over */ \
    memcpy(ptr, \
           vec->data, \
           sizeof(type) * idx); \
    /* overwrite everything else with all elements behind the to-be-deleted-range */ \
    memcpy(ptr + idx, \
           vec->data + (idx + num), \
           sizeof(type) * (vec->size - num - idx)); \
    free(vec->data); \
    vec->data = ptr; \
    vec->size = vec->size - num; \
    return vec->data + idx; \
}

#define _def_vector_find_pred(type) \
vector_iterator_t(type) \
_##type##_vector_find_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred) { \
    vector_for_each(type, vec, elem) { \
        if (pred(*elem, val)) { \
            return elem; \
        } \
    } \
    return vector_end(type, vec); \
}

#define _def_vector_contains_pred(type) \
bool \
_##type##_vector_contains_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred) { \
    vector_for_each(type, vec, elem) { \
        if (pred(*elem, val)) { \
            return true; \
        } \
    } \
    return false; \
}

/**
 * @brief defines all vector-functionality for a vector with `type` as it's value-type.
 *        use this in your source-files.
 */
#define define_vector(type) \
_def_make_vector(type) \
_def_vector_erase_range(type) \
_def_vector_insert_range(type) \
_def_vector_insert_value(type) \
_def_vector_find_pred(type) \
_def_vector_contains_pred(type)

/**
 * @brief if your value-type supports operator==, this is the default predicate that is
 *        used for vector_find() and vector_contains().
 */
#define define_vector_default_compare_predicate(type) \
static inline bool _##type##_vector_default_compare_predicate(type a, type b) { \
    return a == b; \
}

#ifdef BUILD_TESTS
int vector_test(FILE* fno);
#endif

#endif