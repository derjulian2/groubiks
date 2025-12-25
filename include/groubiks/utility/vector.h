
#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

/**
 * @file vector.h
 * @date 24/12/2025
 * @author Julian Benzel
 * @brief type-generic dynamically-sized array data-structure. 
 *        usage:
 *        your_file.h : declare_vector(<type>, <name>)
 *        your_file.c : define_vector(<type>, <name>)
 *
 *        vector_t(<name>) my_vec = make_vector(<name>, ...)
 * @details vectors can be defined for plain-old-data types as well as types that
 *          require a special function for copying, moving, comparing or freeing, such as strings.
 *          for these types, use:
 *          your_file.h : declare_vector_non_pod(<type>, <name>) 
 *          your_file.c : define_vector_non_pod(<type>, <name>)
 *                        <returntype> vector_copy_value_fn(<name>)(<params>) { ... }
 *                        <returntype> vector_move_value_fn(<name>)(<params>) { ... }
 *                        <returntype> vector_free_value_fn(<name>)(<params>) { ... }
 *                        <returntype> vector_compare_values_fn(<name>) { ... }
 *          where you define 4 predicate-functions used by the vector. see the documentation of:
 *          1.) vector_copy_value_fn(<type>)
 *          2.) vector_move_value_fn(<type>)
 *          3.) vector_free_value_fn(<type>)
 *          4.) vector_compare_values_fn(<type>)
 *          for details about the signatures.
 */
 
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#if __STDC_VERSION__ >= 202311L
#include <stdbit.h>
#endif
#ifdef __MSC_VER
#include <intrin.h>
#endif
#ifdef BUILD_TESTS
#include <stdio.h>
#endif

/**
 * @name user-interface macros and types.
 * @{
 */

/**
 * @brief empty vector-instance.
 */
#define null_vector(name) (vector_t(name)){ NULL, 0ll, 0ull }
typedef int vector_result_t;
typedef size_t vector_index_t;
#define VECTOR_SUCCESS  0
#define VECTOR_ERROR   -1
/**
 * @brief type-generic vector and iterator type.
 */
#define vector(name) _##name##_vector
#define vector_t(name) _##name##_vector_t
#define vector_iterator_t(name) _##name##_vector_iterator_t
#define vector_const_iterator_t(name) _##name##_vector_const_iterator_t
/**
 * @brief this macro provides the name of the function that will be used by the vector
 *        to copy two instances of the value-type in internal operations. 
 *        your definition of this copy-method must meet the following signature:
 *          bool vector_copy_value_fn(<name>)(<type>* dest, const <type>* src) { ... }
 *        where a return value of 0 is considered a successful copy and anything else an error.
 */
#define vector_copy_value_fn(name) _##name##_vector_copy_value
/**
 * @brief this macro provides the name of the function that will be used by the vector
 *        to move an instance of the value-type from one place to another in internal operations. 
 *        your definition of this move-method must meet the following signature:
 *          void vector_move_value_fn(<name>)(<type>* dest, <type>* src) { ... }
 *        this operation should not fail.
 */
#define vector_move_value_fn(name) _##name##_vector_move_value
/**
 * @brief this macro provides the name of the function that will be used by the vector
 *        to free an instance of the value-type in internal operations. 
 *        your definition of this free-method must meet the following signature:
 *          void vector_free_value_fn(<name>)(<type>* ptr) { ... }
 *        this operation should not fail.
 */
#define vector_free_value_fn(name) _##name##_vector_free_value
/**
 * @brief this macro provides the name of the function that will be used by the vector
 *        to compare two instances of the value-type in internal operations. 
 *        your definition of this compare-method must meet the following signature:
 *          bool vector_compare_values_fn(<name>)(const <type>* a, const <type>* b) { ... }
 *        where a return value of 1 indicates equality between a and b.
 */
#define vector_compare_values_fn(name) _##name##_vector_compare_values
/**
 * @brief creates a vector of size `num` and can optionally initialize with the data stored in `data`.
 */
#define make_vector(name, data, num, err) _##name##_make_vector(data, num, err)
/**
 * @brief assigns a vector to passed data. no size-adjustment, allocating or copying is performed. the data
 *        is not owned by the vector. this is intended to provide a vector_t(<type>)-wrapper
 *        around your data to use the vector-methods on it.
 */
#define assign_vector(name, data, num) (vector_t(name)){ .data = data, .size = num, .capacity = num }
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
#define vector_resize(name, vec, sz, err) _##name##_vector_resize(vec, sz, err)
/**
 * @brief inserts new elements before the element at `idx`.
 * @returns an iterator to the first newly inserted element, or NULL on failure.
 *          the passed vector will remain unchanged on failure.
 */
#define vector_insert(name, vec, idx, val, err) _##name##_vector_insert_value(vec, idx, val, err)
#define vector_insert_range(name, vec, idx, src, num, err) _##name##_vector_insert(vec, idx, src, num, err)
#define vector_push_back(name, vec, val, err) vector_insert(name, vec, (vec)->size, val, err)
/**
 * @brief erases elements at `idx`.
 */
#define vector_erase_range(name, vec, idx, num, err) _##name##_vector_erase(vec, idx, num, err)
#define vector_erase(name, vec, idx, err) vector_erase_range(name, vec, idx, 1, err)
/**
 * @brief reserves memory for <sz> elements. does nothing if <sz> is smaller than the current capacity.
 */
#define vector_reserve(name, vec, sz, err) _##name##_vector_reserve(vec, sz, err)
/**
 * @brief shrinks buffer to fit exactly to the current element-count. 
 */
#define vector_shrink_to_fit(name, vec, err) _##name##_vector_shrink_to_fit(vec, err)
/**
 * @brief zeroes all elements in the vector, without freeing or reallocating any data. use with caution.
 */
#define vector_zero(vec) memset((vec)->data, 0, sizeof(*(vec)->data) * (vec)->size)
/**
 * @returns true if vector contains element of value `val`
 */
#define vector_contains(name, vec, val) _##name##_vector_contains(vec, val)
/**
 * @returns iterator to the first element of value `val`
 */
#define vector_find(name, vec, val) _##name##_vector_find(vec, val)
/**
 * @returns a newly allocated vector with no duplicate elements.
 */
#define vector_uniques(name, vec) _##name##_vector_uniques(vec)
/**
 * @brief produces an index from an iterator and a vector.
 */
#define vector_index(vec, iter) ((ptrdiff_t)(iter - (vec)->data))
/**
 * @brief convenience macro for vector-traversal.
 */
#define vector_for_each(name, vec, iter)  \
for (vector_iterator_t(name) iter = vector_begin(vec); \
    iter != vector_end(vec);  \
    ++iter)
#define vector_const_for_each(name, vec, iter)  \
for (vector_const_iterator_t(name) iter = vector_begin(vec); \
    iter != vector_end(vec);  \
    ++iter)
/**
 * @brief erases data owned by the vector. can safely free a null-vector.
 */
#define free_vector(name, vec) _##name##_free_vector(vec)

/**
 * @}
 */

/**
 * @brief implementation begins here. beware: ugly macros.
 */

/**
 * @name    utility for sizing.
 * @details this vector scales by allocating to the next-largest 
 *          power-of-two of the currently needed size to hold all elements.
 * @{
 */
#define _vector_grow_cap(sz) 1ull << (1ull + msb(sz))
/**
 * @brief   tries to use fast instructions of bit-manipulation to determine
 *          the location of the most-significant bit of a number.
 * @returns most-significant-bit of n, i.e. n=0b1101 would return 3, as the bit for 2^3 is set.
 *          behaviour for n=0 is undefined.
 */
static inline int msb(uint64_t n) {
#if __STDC_VERSION__ >= 202311L
    return stdc_bit_width(n);
#elif defined(__GNUC__)
    return 63 - __builtin_clzll(n);
#elif defined(__MSC_VER)
    unsigned long long res;
    _BitScanReverse64(&res, n);
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
 * @}
 */

/**
 * @name macro-declarations.
 * @{
 */

 #define _decl_vector_struct(type, name) \
struct _##name##_vector { \
    type* data; \
    size_t size; \
    size_t capacity; \
}; \
typedef struct _##name##_vector _##name##_vector_t; \
typedef type* _##name##_vector_iterator_t; \
typedef const type* _##name##_vector_const_iterator_t;

#define _decl_vector_value_functions_non_pod(type, name) \
bool _##name##_vector_copy_value(type* dest, const type* src); \
void _##name##_vector_move_value(type* dest, type* src); \
void _##name##_vector_free_value(type* ptr); \

#define _decl_vector_internal_functions_non_pod(type, name) \
bool _##name##_vector_copy_values(type* dest, const type* src, size_t num); \
void _##name##_vector_move_values(type* dest, type* src, size_t num); \
void _##name##_vector_free_values(type* ptr, size_t num); \

#define _decl_vector_special_functions(type, name) \
vector_t(name) _##name##_make_vector(const type* data, size_t num, vector_result_t* err); \
void _##name##_free_vector(vector_t(name)* vec);

#define _decl_vector_modifier_functions(type, name) \
vector_iterator_t(name) _##name##_vector_insert(vector_t(name)* vec, vector_index_t idx, const type* src, size_t num, vector_result_t* err); \
vector_iterator_t(name) _##name##_vector_insert_value(vector_t(name)* vec, vector_index_t idx, const type val, vector_result_t* err); \
vector_iterator_t(name) _##name##_vector_erase(vector_t(name)* vec, vector_index_t idx, size_t num, vector_result_t* err); \
void _##name##_vector_resize(vector_t(name)* vec, size_t sz, vector_result_t* err); \
void _##name##_vector_reserve(vector_t(name)* vec, size_t sz, vector_result_t* err); \
void _##name##_vector_shrink_to_fit(vector_t(name)* vec, vector_result_t* err);

#define _decl_vector_search_functions(type, name) \
vector_iterator_t(name) _##name##_vector_find(vector_t(name)* vec, const type val); \
bool _##name##_vector_contains(vector_t(name)* vec, const type val); \
vector_t(name) _##name##_vector_uniques(vector_t(name)* vec); \
bool _##name##_vector_compare_values(const type* a, const type* b);

/**
 * @}
 */

/**
 * @name internal bulk-copying and freeing for non-plain-old-data types:
 * @{
 */

#define _def_vector_internal_functions_non_pod(type, name) \
bool \
_##name##_vector_copy_values(type* dest, const type* src, size_t num) { \
    bool err = false; \
    size_t i; \
    for (i = 0; i < num; ++i) { \
        if (vector_copy_value_fn(name)(&dest[i], &src[i]) != 0) \
        { err = true; break; } \
    } \
    /* cleanup if construction of single elements fails halfway through */ \
    if (err) { \
        _##name##_vector_free_values(dest, i); \
        return VECTOR_ERROR; \
    } \
    return VECTOR_SUCCESS; \
} \
\
void \
_##name##_vector_move_values(type* dest, type* src, size_t num) { \
    for (size_t i = 0; i < num; ++i) { \
        vector_move_value_fn(name)(&dest[i], &src[i]); \
    } \
} \
\
void \
_##name##_vector_free_values(type* ptr, size_t num) { \
    for (size_t i = 0; i < num; ++i) { \
        vector_free_value_fn(name)(&ptr[i]); \
    } \
}

/**
 * @}
 */

/**
 * @name special functions (constructor/free), pod and non-pod versions.
 * @{
 */

#define _def_vector_special_functions_pod(type, name) \
vector_t(name) \
_##name##_make_vector(const type* data, size_t num, vector_result_t* err) { \
    if (!num) { return null_vector(name); } \
    size_t cap = _vector_grow_cap(num); \
    vector_t(name) tmp = { \
        .data = malloc(sizeof(type) * cap), \
        .size = num, \
        .capacity = cap \
    }; \
    if (tmp.data == NULL) { goto error; } \
    if (data) { memcpy(tmp.data, data, sizeof(type) * num); } \
    if (err) { *err = VECTOR_SUCCESS; } \
    return tmp; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return null_vector(name); \
} \
\
void \
_##name##_free_vector(vector_t(name)* vec) { \
    free(vec->data); \
}

#define _def_vector_special_functions_non_pod(type, name) \
vector_t(name) \
_##name##_make_vector(const type* data, size_t num, vector_result_t* err) { \
    if (!num) { return null_vector(name); } \
    size_t cap = _vector_grow_cap(num); \
    vector_t(name) tmp = { \
        .data = malloc(sizeof(type) * cap), \
        .size = num, \
        .capacity = cap \
    }; \
    if (tmp.data == NULL) { goto error; } \
    if (data) { \
        if (_##name##_vector_copy_values(tmp.data, data, num) != 0) { \
            free(tmp.data); goto error; \
        } \
    } \
    if (err) { *err = VECTOR_SUCCESS; } \
    return tmp; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return null_vector(name); \
} \
\
void \
_##name##_free_vector(vector_t(name)* vec) { \
    _##name##_vector_free_values(vec->data, vec->size); \
    free(vec->data); \
}

/**
 * @}
 */

/**
 * @name modifier functions, pod and non-pod versions.
 * @{
 */

#define _def_vector_modifier_functions_pod(type, name) \
vector_iterator_t(name) _##name##_vector_insert(vector_t(name)* vec, \
    vector_index_t idx, \
    const type* src, \
    size_t num, \
    vector_result_t* err) { \
    assert(vec); \
    /* determine if reallocate is required. */ \
    if (vec->size + num > vec->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _vector_grow_cap(vec->size + num); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* copy over elements all existing elements. */ \
        memcpy(ptr, \
               vec->data, \
               sizeof(type) * vec->size); \
        /* free the old buffer and assign. */ \
        free(vec->data); \
        vec->data = ptr; \
        vec->capacity = cap; \
    } \
    /* move all elements after the to-be-inserted range down. */ \
    memmove(vec->data + (idx + num), \
            vec->data + idx, \
            sizeof(type) * (vec->size - idx)); \
    /* copy the to-be-inserted range over. */ \
    memcpy(vec->data + idx, \
           src, \
           sizeof(type) * num); \
    vec->size = vec->size + num; \
success: \
    if (err) { *err = VECTOR_SUCCESS; } \
    return vector_at(vec, idx); \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return NULL; \
} \
\
vector_iterator_t(name) _##name##_vector_insert_value(vector_t(name)* vec, \
    vector_index_t idx, \
    const type val, \
    vector_result_t* err) { \
    return _##name##_vector_insert(vec, idx, &val, 1, err); \
} \
\
vector_iterator_t(name) _##name##_vector_erase(vector_t(name)* vec, \
    vector_index_t idx, \
    size_t num, \
    vector_result_t* err) { \
    assert(vec); \
    assert(num <= vec->size - idx); \
    /* overwrite existing elements with elements after the to-be-erased range. */ \
    memmove(vec->data + idx, \
           vec->data + (idx + num), \
           sizeof(type) * (vec->size - num - idx)); \
    vec->size = vec->size - num; \
    return vector_at(vec, idx); \
} \
\
void _##name##_vector_resize(vector_t(name)* vec, \
    size_t sz, \
    vector_result_t* err) { \
    assert(vec); \
    /* edge-case handling, resize to 0. */ \
    if (!sz) { free_vector(name, vec); *vec = null_vector(name); goto success; } \
    /* determine if reallocate is required. */ \
    if (sz > vec->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _vector_grow_cap(sz); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* copy over existing elements */ \
        memcpy(ptr, vec->data, sizeof(type) * vec->size); \
        /* free the old buffer and assign. */ \
        free(vec->data); \
        vec->data = ptr; \
        vec->capacity = cap; \
    } \
    vec->size = sz; \
success: \
    if (err) { *err = VECTOR_SUCCESS; } \
    return; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return; \
} \
\
void \
_##name##_vector_reserve(vector_t(name)* vec, size_t sz, vector_result_t* err) { \
    assert(vec); \
    /* determine if reallocate is required. */ \
    if (vec->capacity >= sz) { return; } \
    type* ptr = malloc(sizeof(type) * sz); \
    if (ptr == NULL) { goto error; } \
    /* copy over all elements. */ \
    memcpy(ptr, vec->data, sizeof(type) * vec->size); \
    /* free the old buffer and assign. */ \
    free(vec->data); \
    vec->data = ptr; \
    vec->capacity = sz; \
    if (err) { *err = VECTOR_SUCCESS; } \
    return; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return; \
} \
\
void \
_##name##_vector_shrink_to_fit(vector_t(name)* vec, vector_result_t* err) { \
    assert(vec); \
    type* ptr = malloc(sizeof(type) * vec->size); \
    if (ptr == NULL) { goto error; } \
    /* copy over all elements. */ \
    memcpy(ptr, vec->data, sizeof(type) * vec->size); \
    /* free the old buffer and assign. */ \
    free(vec->data); \
    vec->data = ptr; \
    vec->capacity = vec->size; \
    if (err) { *err = VECTOR_SUCCESS; } \
    return; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return; \
}

#define _def_vector_modifier_functions_non_pod(type, name) \
vector_iterator_t(name) _##name##_vector_insert(vector_t(name)* vec, \
    vector_index_t idx, \
    const type* src, \
    size_t num, \
    vector_result_t* err) { \
    assert(vec); \
    /* determine if reallocate is required. */ \
    if (vec->size + num > vec->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _vector_grow_cap(vec->size + num); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* move over elements all existing elements. */ \
        _##name##_vector_move_values(ptr, \
               vec->data, \
               vec->size); \
        /* move all elements after the to-be-inserted range down. */ \
        _##name##_vector_move_values(ptr + (idx + num), \
            ptr + idx, \
            vec->size - idx); \
        /* copy the to-be-inserted range over. */ \
        if (_##name##_vector_copy_values(ptr + idx, src, num) != 0) { free(ptr); goto error; } \
        /* free the old buffer and assign. */ \
        free(vec->data); \
        vec->data = ptr; \
        vec->capacity = cap; \
    } \
    else { \
        /* move all elements after the to-be-inserted range down. */ \
        _##name##_vector_move_values(vec->data + (idx + num), \
            vec->data + idx, \
            vec->size - idx); \
        /* copy the to-be-inserted range over. */ \
        if (_##name##_vector_copy_values(vec->data + idx, src, num) != 0) { \
            /* move elements back to their original positions on copy-error. */ \
            _##name##_vector_move_values(vec->data + idx, \
                vec->data + (idx + num), \
                vec->size - idx); \
            goto error; \
        } \
    } \
    vec->size = vec->size + num; \
success: \
    if (err) { *err = VECTOR_SUCCESS; } \
    return vector_at(vec, idx); \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return NULL; \
} \
\
vector_iterator_t(name) _##name##_vector_insert_value(vector_t(name)* vec, \
    vector_index_t idx, \
    const type val, \
    vector_result_t* err) { \
    return _##name##_vector_insert(vec, idx, &val, 1, err); \
} \
\
vector_iterator_t(name) _##name##_vector_erase(vector_t(name)* vec, \
    vector_index_t idx, \
    size_t num, \
    vector_result_t* err) { \
    assert(vec); \
    assert(num <= vec->size - idx); \
    /* cleanup elements within the to-be-erased range. */ \
    _##name##_vector_free_values(vec->data + idx, num); \
    /* overwrite existing elements with elements after the to-be-erased range. */ \
    _##name##_vector_move_values(vec->data + idx, \
           vec->data + (idx + num), \
           vec->size - num - idx); \
    vec->size = vec->size - num; \
    return vector_at(vec, idx); \
} \
\
void _##name##_vector_resize(vector_t(name)* vec, \
    size_t sz, \
    vector_result_t* err) { \
    assert(vec); \
    /* edge-case handling, resize to 0. */ \
    if (!sz) { free_vector(name, vec); *vec = null_vector(name); goto success; } \
    /* determine if reallocate is required. */ \
    if (sz > vec->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _vector_grow_cap(sz); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* move over existing elements. */ \
        _##name##_vector_move_values(ptr, vec->data, vec->size); \
        /* free the old buffer and assign. */ \
        free(vec->data); \
        vec->data = ptr; \
        vec->capacity = cap; \
    } \
    else if (sz < vec->size) { \
        /* if resize causes a shrink, clean up discarded elements. */ \
        _##name##_vector_free_values(&vec->data[sz], vec->size - sz); \
    } \
    vec->size = sz; \
success: \
    if (err) { *err = VECTOR_SUCCESS; } \
    return; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return; \
} \
void \
_##name##_vector_reserve(vector_t(name)* vec, size_t sz, vector_result_t* err) { \
    assert(vec); \
    /* determine if reallocate is required. */ \
    if (vec->capacity >= sz) { return; } \
    type* ptr = malloc(sizeof(type) * sz); \
    if (ptr == NULL) { goto error; } \
    /* move over all elements. */ \
    _##name##_vector_move_values(ptr, vec->data, vec->size); \
    /* free the old buffer and assign. */ \
    free(vec->data); \
    vec->data = ptr; \
    vec->capacity = sz; \
    if (err) { *err = VECTOR_SUCCESS; } \
    return; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return; \
} \
\
void \
_##name##_vector_shrink_to_fit(vector_t(name)* vec, vector_result_t* err) { \
    assert(vec); \
    type* ptr = malloc(sizeof(type) * vec->size); \
    if (ptr == NULL) { goto error; } \
    /* move over all elements. */ \
    _##name##_vector_move_values(ptr, vec->data, vec->size); \
    /* free the old buffer and assign. */ \
    free(vec->data); \
    vec->data = ptr; \
    vec->capacity = vec->size; \
    if (err) { *err = VECTOR_SUCCESS; } \
    return; \
error: \
    if (err) { *err = VECTOR_ERROR; } \
    return; \
}

/**
 * @}
 */

/**
 * @name search-functions.
 * @{
 */

#define _def_vector_compare_function_pod(type, name) \
bool _##name##_vector_compare_values(const type* a, const type* b) { assert(a && b); return *a == *b; }

#define _def_vector_search_functions(type, name) \
vector_iterator_t(name) \
_##name##_vector_find(vector_t(name)* vec, const type val) { \
    vector_for_each(name, vec, elem) { \
        if (vector_compare_values_fn(name)((const type*)elem, &val)) { \
            return elem; \
        } \
    } \
    return vector_end(vec); \
} \
\
bool \
_##name##_vector_contains(vector_t(name)* vec, const type val) { \
    vector_for_each(name, vec, elem) { \
        if (vector_compare_values_fn(name)((const type*)elem, &val)) { \
            return true; \
        } \
    } \
    return false; \
} \
\
vector_t(name) \
_##name##_vector_uniques(vector_t(name)* vec) { \
    vector_result_t err = VECTOR_SUCCESS; vector_t(name) res = null_vector(name); \
    vector_for_each(name, vec, elem) { \
        if (!vector_contains(name, &res, *elem)) { \
            vector_push_back(name, &res, *elem, &err); \
            if (err != VECTOR_SUCCESS) { goto error; } \
        } \
    } \
    return res; \
error: \
    free_vector(name, &res); \
    return null_vector(name); \
}

/**
 * @}
 */

/**
 * @name user-interface to declare/define vectors of pod and non-pod types.
 * @{
 */

/**
 * @brief declares a vector with <type> as it's value-type.
 *        you can refer to vectors of this type with vector_t(<name>).
 *        name and type may be equivalent, the distinction is useful
 *        for value-types with special-characters or qualifiers as e.g. pointers.
 *
 *        preferably use this in your header-files.
 */
#define declare_vector(type, name) \
_decl_vector_struct(type, name) \
_decl_vector_special_functions(type, name) \
_decl_vector_modifier_functions(type, name) \
_decl_vector_search_functions(type, name)

/**
 * @brief declares a vector with <type> as it's value-type.
 *        you can refer to vectors of this type with vector_t(<name>).
 *        name and type may be equivalent, the distinction is useful
 *        for value-types with special-characters or qualifiers as e.g. pointers.
 *
 *        if you declare a vector with this macro you also want to define
 *        four other functions to let the vector know how your type operates.
 *        see the documentation for:
 *        - vector_copy_value_fn(<name>)
 *        - vector_move_value_fn(<name>)
 *        - vector_free_value_fn(<name>)
 *        - vector_compare_values_fn(<name>)
 *
 *        preferably use this in your header-files.
 */
#define declare_vector_non_pod(type, name) \
_decl_vector_value_functions_non_pod(type, name) \
_decl_vector_internal_functions_non_pod(type, name) \
_decl_vector_struct(type, name) \
_decl_vector_special_functions(type, name) \
_decl_vector_modifier_functions(type, name) \
_decl_vector_search_functions(type, name)

/**
 * @brief defines all vector-functionality for a vector with <type> as it's value-type.
 *        
 *        preferably use this in your source-files.
 */
#define define_vector(type, name) \
_def_vector_compare_function_pod(type, name) \
_def_vector_special_functions_pod(type, name) \
_def_vector_modifier_functions_pod(type, name) \
_def_vector_search_functions(type, name)

/**
 * @brief defines all vector-functionality for a vector with <type> as it's value-type
 *        and treating every instance of <type> as a non-pod type.
 *        this means that copying, freeing and comparing is dependent on your
 *        definitions of these functions.
 *
 *        preferably use this in your source-files.
 */
#define define_vector_non_pod(type, name) \
_def_vector_internal_functions_non_pod(type, name) \
_def_vector_special_functions_non_pod(type, name) \
_def_vector_modifier_functions_non_pod(type, name) \
_def_vector_search_functions(type, name)

/**
 * @}
 */

#ifdef BUILD_TESTS
int vector_test(FILE* fno);
#endif

#endif