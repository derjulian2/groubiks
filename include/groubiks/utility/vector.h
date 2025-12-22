
#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

/**
 * @file vector.h
 * @date 22/12/2025
 * @author Julian Benzel
 * @brief type-generic dynamically-sized array data-structure. 
 *        usage:
 *        your_file.h : declare_vector(<type>)
 *        your_file.c : define_vector(<type>)
 *
 *        vector_t(<type>) my_vec = make_vector(<type>, ...)
 */
 
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#if __STDC_VERSION__ >= 202311L
#include <stdbit.h>
#endif
#ifdef BUILD_TESTS
#include <stdio.h>
#endif
/**
 * @brief uses fast compiler-builtins to determine most-significant bit, with a
 *        fallback-algorithm on unfamiliar compilers.
 * @returns most-significant-bit of n, i.e. n=0b1101 would return 3, as the bit for 2^3 is set.
 *          behaviour for n=0 is undefined.
 */
static inline int msb(uint64_t n) {
#if __STDC_VERSION__ >= 202311L
    return stdc_bit_width(n);
#elifdef __GNUC__
    return 63 - __builtin_clzll(n);
#elifdef __MSC_VER
    unsigned long long res;
    _BitScanReverse64(&res, n)
    return res;
#else
    /**
     * see 'Denis de Bernardy's answer at 
     * https://stackoverflow.com/questions/17027878/algorithm-to-find-the-most-significant-bit
     * slightly adjusted.
     */
    int bitpos = 0;
    while (n != 1)
    { n >>= 1; ++bitpos; }
    return bitpos;
#endif
}
/**
 * @brief empty vector-instance.
 */
#define null_vector(type) (vector_t(type)){ NULL, 0ll, 0ull }
typedef int vector_result_t;
typedef size_t vector_index_t;
#define VECTOR_SUCCESS  0
#define VECTOR_ERROR   -1
/**
 * @brief type-generic vector and iterator type.
 */
#define vector(type) _##type##_vector
#define vector_t(type) _##type##_vector_t
#define vector_iterator_t(type) _##type##_vector_iterator_t
#define vector_comparison_fn_t(type) _##type##_vector_comp_fn_t
/**
 * @brief creates a vector of size `num` and can optionally initialize with the data stored in `data`.
 */
#define make_vector(type, data, num, err) _##type##_make_vector(data, num, err)
/**
 * @brief assigns the passed data for the vector to manage it. no size-adjustment or copying is performed.
 */
#define assign_vector(type, data, num) _##type##_assign_vector(data, num);
/**
 * @brief erases data owned by the vector. can safely free a null-vector.
 */
#define free_vector(vec) free((vec)->data)
/**
 * @brief accessors. begin() at &data[0], end() at &data[size]
 */
#define vector_begin(vec) ((vec)->data)
#define vector_end(vec) ((vec)->data + (vec)->size)
#define vector_at(vec, idx) ((vec)->data + idx)
/**
 * @brief resizes the vector to a fixed size and capacity.
 *        the contents of new elements that may be allocated during a resize are undefined.
 */
#define vector_resize(type, vec, sz, err) _##type##_vector_resize(vec, sz, err)
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
#define vector_zero(vec) memset((vec)->data, 0, sizeof(*(vec)->data) * (vec)->size)
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
 * @returns a newly allocated vector with no duplicate elements.
 */
#define vector_uniques(type, vec) _##type##_vector_uniques_pred(vec, vector_default_compare_predicate(type))
#define vector_uniques_predicate(type, vec, pred) _##type##_vector_uniques_pred(vec, pred)
/**
 * @brief produces an index from an iterator and a vector.
 */
#define vector_make_index(vec, iter) ((ptrdiff_t)(iter - (vec)->data))
/**
 * @brief convenience macro for vector-traversal.
 */
#define vector_for_each(type, vec, name)  \
for (vector_iterator_t(type) name = vector_begin(vec); \
    name != vector_end(vec);  \
    ++name)

/**
 * implementation starts here. beware: very ugly because macros.
 */
#define _decl_vector_struct(type) \
struct _##type##_vector { \
    type* data; \
    size_t size; \
    size_t capacity; \
}; \
typedef struct _##type##_vector _##type##_vector_t; \
typedef type* _##type##_vector_iterator_t; \
typedef bool(*_##type##_vector_comp_fn_t)(type, type);

#define _decl_vector_functions(type) \
vector_t(type) _##type##_make_vector(type* data, size_t num, vector_result_t* err); \
vector_t(type) _##type##_assign_vector(type* data, size_t num); \
vector_iterator_t(type) _##type##_vector_insert(vector_t(type)* vec, vector_index_t idx, type* src, size_t num, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_insert_value(vector_t(type)* vec, vector_index_t idx, type val, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_erase(vector_t(type)* vec, vector_index_t idx, size_t num, vector_result_t* err); \
void _##type##_vector_resize(vector_t(type)* vec, size_t sz, vector_result_t* err); \
vector_iterator_t(type) _##type##_vector_find_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred); \
bool _##type##_vector_contains_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred); \
vector_t(type) _##type##_vector_uniques_pred(vector_t(type)* vec, vector_comparison_fn_t(type) pred);

#define _def_make_vector(type) \
vector_t(type) \
_##type##_make_vector(type* data, size_t num, vector_result_t* err) { \
    vector_result_t res = 0; \
    if (!num) { return null_vector(type); } \
    size_t cap = 1 << (1 + msb(num)); \
    vector_t(type) tmp = { \
        .data = malloc(sizeof(type) * cap), \
        .size = num, \
        .capacity = cap \
    }; \
    res = tmp.data == NULL; \
    if (err) { *err = res; } \
    if (res) { return null_vector(type); } \
    if (data) { memcpy(tmp.data, data, sizeof(type) * num); } \
    return tmp; \
}

#define _def_assign_vector(type) \
vector_t(type) \
_##type##_assign_vector(type* data, size_t num) { \
    return (vector_t(type)) { .data = data, .size = num, .capacity = num }; \
}

#define _def_vector_insert_range(type) \
vector_iterator_t(type) \
_##type##_vector_insert(vector_t(type)* vec, \
        vector_index_t idx, \
        type* src, \
        size_t num, \
        vector_result_t* err) { \
    assert(vec != NULL); \
    size_t cap = vec->capacity; \
    type* ptr = vec->data; \
    /* reallocate to the next power of 2 if needed */ \
    if (vec->size + num > vec->capacity) { \
        cap = (1 << (1 + msb(vec->size + num))); \
        ptr = realloc(vec->data, sizeof(type) * cap); \
        if (ptr != NULL) { \
            if (err != NULL) { *err = VECTOR_SUCCESS; } \
        } \
        else { \
            if (err != NULL) { *err = VECTOR_ERROR; } \
            return NULL; \
        } \
    } \
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
    vec->capacity = cap; \
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
    /* overwrite with all elements behind the to-be-deleted-range */ \
    memcpy(vec->data + idx, \
           vec->data + (idx + num), \
           sizeof(type) * (vec->size - num - idx)); \
    vec->size = vec->size - num; \
    return vec->data + idx; \
}

#define _def_vector_resize(type) \
void \
_##type##_vector_resize(vector_t(type)* vec, size_t sz, vector_result_t* err) { \
    assert(vec != NULL); \
    type* ptr = realloc(vec->data, sizeof(type) * sz); \
    if (ptr != NULL) { \
        if (err != NULL) { *err = VECTOR_SUCCESS; } \
    } \
    else { \
        if (err != NULL) { *err = VECTOR_ERROR; } \
        return; \
    } \
    vec->data = ptr; \
    vec->size = sz; \
    vec->capacity = sz; \
}

#define _def_vector_find_pred(type) \
vector_iterator_t(type) \
_##type##_vector_find_pred(vector_t(type)* vec, type val, vector_comparison_fn_t(type) pred) { \
    vector_for_each(type, vec, elem) { \
        if (pred(*elem, val)) { \
            return elem; \
        } \
    } \
    return vector_end(vec); \
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

#define _def_vector_uniques_pred(type) \
vector_t(type) \
_##type##_vector_uniques_pred(vector_t(type)* vec, vector_comparison_fn_t(type) pred) { \
    vector_result_t err = VECTOR_SUCCESS; vector_t(type) res = null_vector(type); \
    vector_for_each(type, vec, elem) { \
        if (!vector_contains_predicate(type, &res, *elem, pred)) { \
            vector_push_back(type, &res, *elem, &err); \
            if (err != VECTOR_SUCCESS) { goto error; } \
        } \
    } \
    return res; \
error: \
    free_vector(&res); \
    return null_vector(type); \
}

/**
 * @brief declares a vector with `type` as it's value-type.
 *        use this in your header-files.
 */
#define declare_vector(type) \
_decl_vector_struct(type) \
_decl_vector_functions(type)

/**
 * @brief defines all vector-functionality for a vector with `type` as it's value-type.
 *        use this in your source-files.
 */
#define define_vector(type) \
_def_make_vector(type) \
_def_assign_vector(type) \
_def_vector_erase_range(type) \
_def_vector_resize(type) \
_def_vector_insert_range(type) \
_def_vector_insert_value(type) \
_def_vector_find_pred(type) \
_def_vector_contains_pred(type) \
_def_vector_uniques_pred(type)

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