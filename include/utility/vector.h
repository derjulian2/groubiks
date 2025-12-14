
#ifndef CSNAKE_VECTOR_H
#define CSNAKE_VECTOR_H

/**
 * @file stack.h
 * @date 8/11/2025
 * @author Julian Benzel
 * @brief type-generic inplace-vector data-structure with a fixed capacity based on macros.
 */

#include <stdlib.h>
#include <string.h>

#define vector_t(type) type##_vector_t
#define vector_default(type) (type##_vector_t) { 0ull, 0ull, NULL }
#define vector_construct(type, vec_ptr, cap) type##_vector_t_init(vec_ptr, cap)
#define vector_construct_from_array(type, vec_ptr, arr, size, cap) type##_vector_t_init_arr(vec_ptr, arr, size, cap)
#define vector_destroy(type, vec_ptr) type##_vector_t_destroy(vec_ptr)

#define vector_begin(type, vec_ptr) &(*vec_ptr).data[0]
#define vector_end(type, vec_ptr) &(*vec_ptr).data[(*vec_ptr).size]

#define vector_front(type, vec_ptr) (*vec_ptr).data[0]
#define vector_back(type, vec_ptr) (*vec_ptr).data[(*vec_ptr).size - 1]

#define vector_push_back(type, vec_ptr, val) type##_vector_t_push_back(vec_ptr, val)
#define vector_erase(type, vec_ptr, idx) type##_vector_t_erase(vec_ptr, idx)

#define vector_find(type, vec_ptr, val, pred) type##_vector_t_find(vec_ptr, val, pred)
#define vector_contains(type, vec_ptr, val, pred) type##_vector_t_contains(vec_ptr, val, pred)

#define declare_vector(type) \
typedef struct \
{ \
    size_t size; \
    size_t capacity; \
    type* data; \
} type##_vector_t; \
typedef int(*type##_vector_t_comp_fptr)(const type, const type); \
int type##_vector_t_init(vector_t(type)* const vec, size_t cap); \
int type##_vector_t_init_arr(vector_t(type)* const vec, const type* const arr, size_t num, size_t cap); \
void type##_vector_t_destroy(vector_t(type)* const vec); \
type* const type##_vector_t_push_back(vector_t(type)* const vec, const type val); \
type* const type##_vector_t_erase(vector_t(type)* const vec, size_t idx); \
type* type##_vector_t_find(vector_t(type)* const vec, const type val, type##_vector_t_comp_fptr pred); \
int type##_vector_t_contains(vector_t(type)* const vec, const type val, type##_vector_t_comp_fptr pred);

#define define_vector(type) \
int type##_vector_t_init(vector_t(type)* const vec, size_t cap) \
{ \
    vector_t(type) tmp = \
    { \
        0, \
        cap, \
        calloc(cap, sizeof(type)) \
    }; \
    if (!tmp.data) \
    { return -1; } \
    *vec = tmp; \
    return 0; \
} \
\
int type##_vector_t_init_arr(vector_t(type)* const vec, const type* const arr, size_t num, size_t cap) \
{ \
    vector_t(type) tmp = \
    { \
        num, \
        cap, \
        calloc(cap, sizeof(type)) \
    }; \
    if (!tmp.data) \
    { return -1; } \
    memcpy(tmp.data, arr, num * sizeof(type)); \
    *vec = tmp; \
    return 0; \
} \
\
void type##_vector_t_destroy(vector_t(type)* const vec) \
{ free(vec->data); } \
\
type* const type##_vector_t_push_back(vector_t(type)* const vec, const type val) \
{ \
    if (vec->size < vec->capacity) \
    { type* const ptr = &vec->data[vec->size++]; *ptr = val; return ptr; } \
    return vector_end(type, vec); \
} \
\
type* const type##_vector_t_erase(vector_t(type)* const vec, size_t idx) \
{ \
    if (idx < vec->size) \
    { \
        memmove(&vec->data[idx], &vec->data[idx + 1], sizeof(type) * (--vec->size - idx)); \
        return &vec->data[idx]; \
    } \
    return vector_end(type, vec); \
} \
type* type##_vector_t_find(vector_t(type)* const vec, const type val, type##_vector_t_comp_fptr pred) \
{ \
    for (type* i = vector_begin(type, vec); i != vector_end(type, vec); ++i) \
    { if (pred(*i, val)) { return i; } } \
    return vector_end(type, vec); \
} \
int type##_vector_t_contains(vector_t(type)* const vec, const type val, type##_vector_t_comp_fptr pred) \
{ return vector_find(type, vec, val, pred) != vector_end(type, vec); }

#endif