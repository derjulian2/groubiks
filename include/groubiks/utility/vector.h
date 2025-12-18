
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
/**
 * @brief creates a vector of size `num` and can optionally initialize with the data stored in `data`.
 */
#define make_vector(type, data, num, err) _##type##_make_vector(data, num, err)
#define free_vector(type, vec) _##type##_free_vector(vec)
/**
 * @brief accessors.
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

#define vector_for_each(type, vec, name)  \
for (vector_iterator_t(type) name = vector_begin(type, vec); \
    name != vector_end(type, vec);  \
    ++name)

#define _decl_vector_struct(type) \
typedef struct { \
    type* data; \
    size_t size; \
} _##type##_vector_t; \
typedef type* _##type##_vector_iterator_t;

#define _decl_vector_functions(type) \
vector_t(type) _##type##_make_vector(type* data, size_t num, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_insert(vector_t(type)* vec, vector_index_t idx, type* src, size_t num, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_insert_value(vector_t(type)* vec, vector_index_t idx, type val, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_erase(vector_t(type)* vec, vector_index_t idx, size_t num, vector_result_t* err); \
void _##type##_free_vector(vector_t(type)* vec);

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

#define _def_free_vector(type) \
void \
_##type##_free_vector(vector_t(type)* vec) { \
    assert(vec != NULL); \
    free(vec->data); \
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

#define define_vector(type) \
_def_make_vector(type) \
_def_free_vector(type) \
_def_vector_erase_range(type) \
_def_vector_insert_range(type) \
_def_vector_insert_value(type)

#ifdef BUILD_TESTS
int vector_test(FILE* fno);
#endif

#endif