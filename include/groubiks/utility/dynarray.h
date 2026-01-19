
#ifndef UTIL_DYNARRAY_H
#define UTIL_DYNARRAY_H

/**
 * @file dynarray.h
 * @date 30/12/2025
 * @author Julian Benzel
 * @brief type-generic dynamically-sized array data-structure implemented with macros.
 *        usage:
 *        your_file.h : declare_dynarray(<type>, <name>)
 *        your_file.c : define_dynarray(<type>, <name>, <optional params>)
 *
 *        dynarray_t(<name>) my_dyn = make_dynarray(<name>, ...)
 *
 *        const-qualified value-types are not supported.
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
 * @brief empty dynarray-instance.
 */
#define null_dynarray(name) (dynarray_t(name)){ NULL, 0ll, 0ull }
typedef int dynarray_result_t;
typedef size_t dynarray_index_t;
#define DYNARRAY_SUCCESS  0
#define DYNARRAY_ERROR   -1
/**
 * @brief type-generic dynarray and iterator type.
 */
#define dynarray(name) _##name##_dynarray
#define dynarray_t(name) _##name##_dynarray_t
#define dynarray_iterator_t(name) _##name##_dynarray_iterator_t
#define dynarray_const_iterator_t(name) _##name##_dynarray_const_iterator_t
/**
 * @brief creates a dynarray of size `num` and can optionally initialize with the data stored in `data`.
 */
#define make_dynarray(name, data, num, err) _##name##_make_dynarray(data, num, err)
/**
 * @brief assigns a dynarray to passed data. no size-adjustment, allocating or copying is performed. the data
 *        is not owned by the dynarray. this is intended to provide a dynarray_t(<type>)-wrapper
 *        around your data to use the dynarray-methods on it.
 */
#define assign_dynarray(name, data, num) (dynarray_t(name)){ .data = data, .size = num, .capacity = num }
/**
 * @brief accessors. begin() at &data[0], end() at &data[size]
 */
#define dynarray_begin(dyn) ((dyn)->data)
#define dynarray_end(dyn) ((dyn)->data + (dyn)->size)
#define dynarray_at(dyn, idx) ((dyn)->data + idx)
/**
 * @brief resizes the dynarray to a fixed size and capacity.
 *        the contents of new elements that may be allocated during a resize are undefined.
 */
#define dynarray_resize(name, dyn, sz, err) _##name##_dynarray_resize(dyn, sz, err)
/**
 * @brief inserts new elements before the element at `idx`.
 * @returns an iterator to the first newly inserted element, or NULL on failure.
 *          the passed dynarray will remain unchanged on failure.
 */
#define dynarray_insert(name, dyn, idx, val, err) _##name##_dynarray_insert_value(dyn, idx, val, err)
#define dynarray_insert_range(name, dyn, idx, src, num, err) _##name##_dynarray_insert(dyn, idx, src, num, err)
#define dynarray_push_back(name, dyn, val, err) dynarray_insert(name, dyn, (dyn)->size, val, err)
/**
 * @brief erases elements at `idx`.
 */
#define dynarray_erase_range(name, dyn, idx, num, err) _##name##_dynarray_erase(dyn, idx, num, err)
#define dynarray_erase(name, dyn, idx, err) dynarray_erase_range(name, dyn, idx, 1, err)
/**
 * @brief reserves memory for <sz> elements. does nothing if <sz> is smaller than the current capacity.
 */
#define dynarray_reserve(name, dyn, sz, err) _##name##_dynarray_reserve(dyn, sz, err)
/**
 * @brief shrinks buffer to fit exactly to the current element-count. 
 */
#define dynarray_shrink_to_fit(name, dyn, err) _##name##_dynarray_shrink_to_fit(dyn, err)
/**
 * @brief zeroes all elements in the dynarray, without freeing or reallocating any data. use with caution.
 */
#define dynarray_zero(dyn) memset((dyn)->data, 0, sizeof(*(dyn)->data) * (dyn)->size)
/**
 * @returns true if dynarray contains element of value `val`
 */
#define dynarray_contains(name, dyn, val) _##name##_dynarray_contains(dyn, val)
/**
 * @returns iterator to the first element of value `val`
 */
#define dynarray_find(name, dyn, val) _##name##_dynarray_find(dyn, val)
/**
 * @returns a newly allocated dynarray with no duplicate elements.
 */
#define dynarray_uniques(name, dyn, err) _##name##_dynarray_uniques(dyn, err)
#define dynarray_uniques_from_range(name, data, num, err) _##name##_dynarray_uniques_from_range(data, num, err)
/**
 * @brief produces an index from an iterator and a dynarray.
 */
#define dynarray_index(dyn, iter) ((ptrdiff_t)(iter - (dyn)->data))
/**
 * @brief convenience macro for dynarray-traversal.
 */
#define dynarray_for_each(name, dyn, iter)  \
for (dynarray_iterator_t(name) iter = dynarray_begin(dyn); \
    iter != dynarray_end(dyn);  \
    ++iter)
#define dynarray_const_for_each(name, dyn, iter)  \
for (dynarray_const_iterator_t(name) iter = dynarray_begin(dyn); \
    iter != dynarray_end(dyn);  \
    ++iter)
/**
 * @brief erases data owned by the dynarray. can safely free a null-dynarray.
 */
#define free_dynarray(name, dyn) _##name##_free_dynarray(dyn)

/**
 * @}
 */

/**
 * @brief implementation begins here. beware: ugly macros.
 */

/**
 * @name    utility for sizing.
 * @details this dynarray scales by allocating to the next-largest 
 *          power-of-two of the currently needed size to hold all elements.
 * @{
 */
#define _dynarray_grow_cap(sz) 1ull << (1ull + msb(sz))
/**
 * @brief   tries to use fast instructions of bit-manipulation to determine
 *          the location of the most-significant bit of a number.
 * @returns most-significant-bit of n, i.e. n=0b1101 would return 3, as the bit for 2^3 is set.
 *          behaviour for n=0 is undefined.
 */
static inline int msb(uint64_t n) {
#if defined(__GNUC__)
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

#define _decl_dynarray_struct(type, name) \
struct _##name##_dynarray { \
    type* data; \
    size_t size; \
    size_t capacity; \
}; \
typedef struct _##name##_dynarray _##name##_dynarray_t; \
typedef type* _##name##_dynarray_iterator_t; \
typedef const type* _##name##_dynarray_const_iterator_t; \
typedef dynarray_result_t(* const _##name##_dynarray_copy_fn_t)(type*, const type*); \
typedef void(* const _##name##_dynarray_move_fn_t)(type*, type*); \
typedef void(* const _##name##_dynarray_free_fn_t)(type*); \
typedef bool(* const _##name##_dynarray_comp_fn_t)(const type*, const type*);

/**
 * @brief global, per-name function-pointers to user-provided functionality.
 * @details some of these are declared but not defined for pod-types,
 *          however the define_dynarray()-macro is constructed to not allow
 *          any scenario in which a function is used but not defined.
 *          if the user passes NULL, that is allowed for the comparison-function
 *          (if the value-type is not comparable), but this will hit an assert
 *          if any search-function is called, because it requires a comparable type.
 */
#define _decl_dynarray_fn_ptrs(type, name) \
extern dynarray_result_t(* const _##name##_dynarray_copy_fn)(type*, const type*); \
extern void(* const _##name##_dynarray_move_fn)(type*, type*); \
extern void(* const _##name##_dynarray_free_fn)(type*); \
extern bool(* const _##name##_dynarray_comp_fn)(const type*, const type*);

#define _decl_dynarray_internal_fns(type, name) \
bool _##name##_dynarray_copy_values(type* dest, const type* src, size_t num); \
void _##name##_dynarray_move_values(type* dest, type* src, size_t num); \
void _##name##_dynarray_free_values(type* ptr, size_t num); \

#define _decl_dynarray_special_fns(type, name) \
dynarray_t(name) _##name##_make_dynarray(const type* data, size_t num, dynarray_result_t* err); \
void _##name##_free_dynarray(dynarray_t(name)* dyn);

#define _decl_dynarray_modifier_fns(type, name) \
dynarray_iterator_t(name) _##name##_dynarray_insert(dynarray_t(name)* dyn, dynarray_index_t idx, const type* src, size_t num, dynarray_result_t* err); \
dynarray_iterator_t(name) _##name##_dynarray_insert_value(dynarray_t(name)* dyn, dynarray_index_t idx, const type val, dynarray_result_t* err); \
dynarray_iterator_t(name) _##name##_dynarray_erase(dynarray_t(name)* dyn, dynarray_index_t idx, size_t num, dynarray_result_t* err); \
void _##name##_dynarray_resize(dynarray_t(name)* dyn, size_t sz, dynarray_result_t* err); \
void _##name##_dynarray_reserve(dynarray_t(name)* dyn, size_t sz, dynarray_result_t* err); \
void _##name##_dynarray_shrink_to_fit(dynarray_t(name)* dyn, dynarray_result_t* err);

#define _decl_dynarray_search_fns(type, name) \
dynarray_iterator_t(name) _##name##_dynarray_find(dynarray_t(name)* dyn, const type val); \
bool _##name##_dynarray_contains(dynarray_t(name)* dyn, const type val); \
dynarray_t(name) _##name##_dynarray_uniques(dynarray_t(name)* dyn, dynarray_result_t* err); \
dynarray_t(name) _##name##_dynarray_uniques_from_range(const type* data, size_t num, dynarray_result_t* err);

/**
 * @}
 */

/**
 * @name defining user-provided function-pointers, 
 *       internal bulk-copying and freeing for non-plain-old-data types:
 * @{
 */

/* kind-generic assign for function-pointers. 'kind' can be 'copy'/'move'/'free'/'comp' */
#define _def_dynarray_fn(name, kind, fn) \
_##name##_dynarray_##kind##_fn_t _##name##_dynarray_##kind##_fn = fn;

#define _def_dynarray_internal_copy(type, name) \
bool \
_##name##_dynarray_copy_values(type* dest, const type* src, size_t num) { \
    assert(_##name##_dynarray_copy_fn != NULL); \
    bool err = false; \
    size_t i; \
    for (i = 0; i < num; ++i) { \
        if (_##name##_dynarray_copy_fn(&dest[i], &src[i]) != DYNARRAY_SUCCESS) \
        { err = true; break; } \
    } \
    /* cleanup if construction of single elements fails halfway through */ \
    if (err) { \
        _##name##_dynarray_free_values(dest, i); \
        return DYNARRAY_ERROR; \
    } \
    return DYNARRAY_SUCCESS; \
} \

#define _def_dynarray_internal_move(type, name) \
void \
_##name##_dynarray_move_values(type* dest, type* src, size_t num) { \
    assert(_##name##_dynarray_move_fn != NULL); \
    /* on overlap, do a backwards-loop. */ \
    if (dest < src && (dest + num >= src) || \
        dest > src && (src + num >= dest)) { \
        for (size_t i = 0; i < num; ++i) { \
            _##name##_dynarray_move_fn(&dest[num - i - 1], &src[num - i - 1]); \
        } \
    } \
    else { \
        for (size_t i = 0; i < num; ++i) { \
            _##name##_dynarray_move_fn(&dest[i], &src[i]); \
        } \
    } \
} \

#define _def_dynarray_internal_move_fast(type, name) \
void \
_##name##_dynarray_move_values(type* dest, type* src, size_t num) { \
    memmove(dest, src, sizeof(type) * num); \
}

#define _def_dynarray_internal_free(type, name) \
void \
_##name##_dynarray_free_values(type* ptr, size_t num) { \
    assert(_##name##_dynarray_free_fn != NULL); \
    for (size_t i = 0; i < num; ++i) { \
        _##name##_dynarray_free_fn(&ptr[i]); \
    } \
}

/**
 * @}
 */

/**
 * @name special functions (constructor/free), pod and non-pod versions.
 * @{
 */

#define _def_dynarray_special_fns(type, name) \
dynarray_t(name) \
_##name##_make_dynarray(const type* data, size_t num, dynarray_result_t* err) { \
    if (!num) { return null_dynarray(name); } \
    size_t cap = _dynarray_grow_cap(num); \
    dynarray_t(name) tmp = { \
        .data = malloc(sizeof(type) * cap), \
        .size = 0ull, \
        .capacity = cap \
    }; \
    if (tmp.data == NULL) { goto error; } \
    if (data) { \
        memcpy(tmp.data, data, sizeof(type) * num); \
        tmp.size = num; \
    } \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return tmp; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return null_dynarray(name); \
} \
\
void \
_##name##_free_dynarray(dynarray_t(name)* dyn) { \
    free(dyn->data); \
}

#define _def_dynarray_special_fns_non_pod(type, name) \
dynarray_t(name) \
_##name##_make_dynarray(const type* data, size_t num, dynarray_result_t* err) { \
    if (!num) { return null_dynarray(name); } \
    size_t cap = _dynarray_grow_cap(num); \
    dynarray_t(name) tmp = { \
        .data = malloc(sizeof(type) * cap), \
        .size = 0ull, \
        .capacity = cap \
    }; \
    if (tmp.data == NULL) { goto error; } \
    if (data) { \
        if (_##name##_dynarray_copy_values(tmp.data, data, num) != 0) { \
            free(tmp.data); goto error; \
        } \
        tmp.size = num; \
    } \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return tmp; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return null_dynarray(name); \
} \
\
void \
_##name##_free_dynarray(dynarray_t(name)* dyn) { \
    _##name##_dynarray_free_values(dyn->data, dyn->size); \
    free(dyn->data); \
}

/**
 * @}
 */

/**
 * @name modifier functions, pod and non-pod versions.
 * @{
 */

#define _def_dynarray_modifier_fns(type, name) \
dynarray_iterator_t(name) _##name##_dynarray_insert(dynarray_t(name)* dyn, \
    dynarray_index_t idx, \
    const type* src, \
    size_t num, \
    dynarray_result_t* err) { \
    assert(dyn); \
    /* determine if reallocate is required. */ \
    if (dyn->size + num > dyn->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _dynarray_grow_cap(dyn->size + num); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* copy over elements all existing elements. */ \
        memcpy(ptr, \
               dyn->data, \
               sizeof(type) * dyn->size); \
        /* free the old buffer and assign. */ \
        free(dyn->data); \
        dyn->data = ptr; \
        dyn->capacity = cap; \
    } \
    /* move all elements after the to-be-inserted range down. */ \
    memmove(dyn->data + (idx + num), \
            dyn->data + idx, \
            sizeof(type) * (dyn->size - idx)); \
    /* copy the to-be-inserted range over. */ \
    memcpy(dyn->data + idx, \
           src, \
           sizeof(type) * num); \
    dyn->size = dyn->size + num; \
success: \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return dynarray_at(dyn, idx); \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return NULL; \
} \
\
dynarray_iterator_t(name) _##name##_dynarray_insert_value(dynarray_t(name)* dyn, \
    dynarray_index_t idx, \
    const type val, \
    dynarray_result_t* err) { \
    return _##name##_dynarray_insert(dyn, idx, &val, 1, err); \
} \
\
dynarray_iterator_t(name) _##name##_dynarray_erase(dynarray_t(name)* dyn, \
    dynarray_index_t idx, \
    size_t num, \
    dynarray_result_t* err) { \
    assert(dyn); \
    assert(num <= dyn->size - idx); \
    /* overwrite existing elements with elements after the to-be-erased range. */ \
    memmove(dyn->data + idx, \
           dyn->data + (idx + num), \
           sizeof(type) * (dyn->size - num - idx)); \
    dyn->size = dyn->size - num; \
    return dynarray_at(dyn, idx); \
} \
\
void _##name##_dynarray_resize(dynarray_t(name)* dyn, \
    size_t sz, \
    dynarray_result_t* err) { \
    assert(dyn); \
    /* edge-case handling, resize to 0. */ \
    if (!sz) { free_dynarray(name, dyn); *dyn = null_dynarray(name); goto success; } \
    /* determine if reallocate is required. */ \
    if (sz > dyn->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _dynarray_grow_cap(sz); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* copy over existing elements */ \
        memcpy(ptr, dyn->data, sizeof(type) * dyn->size); \
        /* free the old buffer and assign. */ \
        free(dyn->data); \
        dyn->data = ptr; \
        dyn->capacity = cap; \
    } \
    dyn->size = sz; \
success: \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return; \
} \
\
void \
_##name##_dynarray_reserve(dynarray_t(name)* dyn, size_t sz, dynarray_result_t* err) { \
    assert(dyn); \
    /* determine if reallocate is required. */ \
    if (dyn->capacity >= sz) { return; } \
    type* ptr = malloc(sizeof(type) * sz); \
    if (ptr == NULL) { goto error; } \
    /* copy over all elements. */ \
    memcpy(ptr, dyn->data, sizeof(type) * dyn->size); \
    /* free the old buffer and assign. */ \
    free(dyn->data); \
    dyn->data = ptr; \
    dyn->capacity = sz; \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return; \
} \
\
void \
_##name##_dynarray_shrink_to_fit(dynarray_t(name)* dyn, dynarray_result_t* err) { \
    assert(dyn); \
    type* ptr = malloc(sizeof(type) * dyn->size); \
    if (ptr == NULL) { goto error; } \
    /* copy over all elements. */ \
    memcpy(ptr, dyn->data, sizeof(type) * dyn->size); \
    /* free the old buffer and assign. */ \
    free(dyn->data); \
    dyn->data = ptr; \
    dyn->capacity = dyn->size; \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return; \
}

#define _def_dynarray_modifier_fns_non_pod(type, name) \
dynarray_iterator_t(name) _##name##_dynarray_insert(dynarray_t(name)* dyn, \
    dynarray_index_t idx, \
    const type* src, \
    size_t num, \
    dynarray_result_t* err) { \
    assert(dyn); \
    /* determine if reallocate is required. */ \
    if (dyn->size + num > dyn->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _dynarray_grow_cap(dyn->size + num); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* copy over elements all existing elements. */ \
        if (_##name##_dynarray_copy_values(ptr, (const type*)dyn->data, dyn->size) != DYNARRAY_SUCCESS) { \
            free(ptr); \
            goto error; \
        }; \
        /* move all elements after the to-be-inserted range down. */ \
        _##name##_dynarray_move_values(ptr + (idx + num), ptr + idx, dyn->size - idx); \
        /* copy the to-be-inserted range over. */ \
        if (_##name##_dynarray_copy_values(ptr + idx, src, num) != DYNARRAY_SUCCESS) { \
            _##name##_dynarray_free_values(ptr, dyn->size); \
            free(ptr); \
            goto error; \
        } \
        _##name##_dynarray_free_values(dyn->data, dyn->size); \
        /* free the old buffer and assign. */ \
        free(dyn->data); \
        dyn->data = ptr; \
        dyn->capacity = cap; \
    } \
    else { \
        /* move all elements after the to-be-inserted range down. */ \
        _##name##_dynarray_move_values(dyn->data + (idx + num), \
            dyn->data + idx, \
            dyn->size - idx); \
        /* copy the to-be-inserted range over. */ \
        if (_##name##_dynarray_copy_values(dyn->data + idx, src, num) != DYNARRAY_SUCCESS) { \
            /* move elements back to their original positions on copy-error. */ \
            _##name##_dynarray_move_values(dyn->data + idx, \
                dyn->data + (idx + num), \
                dyn->size - idx); \
            goto error; \
        } \
    } \
    dyn->size = dyn->size + num; \
success: \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return dynarray_at(dyn, idx); \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return NULL; \
} \
\
dynarray_iterator_t(name) _##name##_dynarray_insert_value(dynarray_t(name)* dyn, \
    dynarray_index_t idx, \
    const type val, \
    dynarray_result_t* err) { \
    return _##name##_dynarray_insert(dyn, idx, &val, 1, err); \
} \
\
dynarray_iterator_t(name) _##name##_dynarray_erase(dynarray_t(name)* dyn, \
    dynarray_index_t idx, \
    size_t num, \
    dynarray_result_t* err) { \
    assert(dyn); \
    assert(num <= dyn->size - idx); \
    /* cleanup elements within the to-be-erased range. */ \
    _##name##_dynarray_free_values(dyn->data + idx, num); \
    /* overwrite existing elements with elements after the to-be-erased range. */ \
    _##name##_dynarray_move_values(dyn->data + idx, \
           dyn->data + (idx + num), \
           dyn->size - num - idx); \
    dyn->size = dyn->size - num; \
    return dynarray_at(dyn, idx); \
} \
\
void _##name##_dynarray_resize(dynarray_t(name)* dyn, \
    size_t sz, \
    dynarray_result_t* err) { \
    assert(dyn); \
    /* edge-case handling, resize to 0. */ \
    if (!sz) { free_dynarray(name, dyn); *dyn = null_dynarray(name); goto success; } \
    /* determine if reallocate is required. */ \
    if (sz > dyn->capacity) { \
        /* determine new capacity and try to allocate for it. */ \
        size_t cap = _dynarray_grow_cap(sz); \
        type* ptr = malloc(sizeof(type) * cap); \
        if (ptr == NULL) { goto error; } \
        /* move over existing elements. */ \
        _##name##_dynarray_move_values(ptr, dyn->data, dyn->size); \
        /* free the old buffer and assign. */ \
        free(dyn->data); \
        dyn->data = ptr; \
        dyn->capacity = cap; \
    } \
    else if (sz < dyn->size) { \
        /* if resize causes a shrink, clean up discarded elements. */ \
        _##name##_dynarray_free_values(&dyn->data[sz], dyn->size - sz); \
    } \
    dyn->size = sz; \
success: \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return; \
} \
void \
_##name##_dynarray_reserve(dynarray_t(name)* dyn, size_t sz, dynarray_result_t* err) { \
    assert(dyn); \
    /* determine if reallocate is required. */ \
    if (dyn->capacity >= sz) { return; } \
    type* ptr = malloc(sizeof(type) * sz); \
    if (ptr == NULL) { goto error; } \
    /* move over all elements. */ \
    _##name##_dynarray_move_values(ptr, dyn->data, dyn->size); \
    /* free the old buffer and assign. */ \
    free(dyn->data); \
    dyn->data = ptr; \
    dyn->capacity = sz; \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return; \
} \
\
void \
_##name##_dynarray_shrink_to_fit(dynarray_t(name)* dyn, dynarray_result_t* err) { \
    assert(dyn); \
    type* ptr = malloc(sizeof(type) * dyn->size); \
    if (ptr == NULL) { goto error; } \
    /* move over all elements. */ \
    _##name##_dynarray_move_values(ptr, dyn->data, dyn->size); \
    /* free the old buffer and assign. */ \
    free(dyn->data); \
    dyn->data = ptr; \
    dyn->capacity = dyn->size; \
    if (err) { *err = DYNARRAY_SUCCESS; } \
    return; \
error: \
    if (err) { *err = DYNARRAY_ERROR; } \
    return; \
}

/**
 * @}
 */

/**
 * @name comparison and search-functions.
 * @{
 */

#define _def_dynarray_comp_fn_default(type, name) \
bool _##name##_dynarray_default_comp_fn(const type* a, const type* b) { assert(a && b); return *a == *b; } \
_##name##_dynarray_comp_fn_t _##name##_dynarray_comp_fn = &_##name##_dynarray_default_comp_fn;

#define _def_dynarray_search_fns(type, name) \
dynarray_iterator_t(name) \
_##name##_dynarray_find(dynarray_t(name)* dyn, const type val) { \
    assert(_##name##_dynarray_comp_fn != NULL); \
    dynarray_for_each(name, dyn, elem) { \
        if (_##name##_dynarray_comp_fn((const type*)elem, &val)) { \
            return elem; \
        } \
    } \
    return dynarray_end(dyn); \
} \
\
bool \
_##name##_dynarray_contains(dynarray_t(name)* dyn, const type val) { \
    assert(_##name##_dynarray_comp_fn != NULL); \
    dynarray_for_each(name, dyn, elem) { \
        if (_##name##_dynarray_comp_fn((const type*)elem, &val)) { \
            return true; \
        } \
    } \
    return false; \
} \
\
dynarray_t(name) \
_##name##_dynarray_uniques(dynarray_t(name)* dyn, dynarray_result_t* err) { \
    return dynarray_uniques_from_range(name, (const type*)dyn->data, dyn->size, err); \
} \
\
dynarray_t(name) \
_##name##_dynarray_uniques_from_range(const type* data, size_t num, dynarray_result_t* err) { \
    dynarray_t(name) res = make_dynarray(name, NULL, num, err); \
    if (*err != DYNARRAY_SUCCESS) { goto error; } \
    for (size_t i = 0; i < num; ++i) { \
        if (!dynarray_contains(name, &res, data[i])) { \
            /* push-back cannot fail, space is reserved. */ \
            dynarray_push_back(name, &res, data[i], NULL); \
        } \
    } \
    return res; \
error: \
    free_dynarray(name, &res); \
    return null_dynarray(name); \
}

/**
 * @}
 */

/**
 * @name user-interface to declare/define dynarrays of pod and non-pod types.
 * @{
 */

/**
 * @brief declares a dynarray with <type> as it's value-type.
 *        you can refer to dynarrays of this type with dynarray_t(<name>).
 *        name and type may be equivalent, the distinction is useful
 *        for value-types with special-characters or qualifiers as e.g. pointers.
 *
 *        preferably use this in your header-files.
 */
#define declare_dynarray(type, name) \
_decl_dynarray_struct(type, name) \
_decl_dynarray_fn_ptrs(type, name) \
_decl_dynarray_internal_fns(type, name) \
_decl_dynarray_special_fns(type, name) \
_decl_dynarray_modifier_fns(type, name) \
_decl_dynarray_search_fns(type, name)

/* utility-macros for overloading */
#define _dynarray_expand(x) x
#define _dynarray_select_macro(_1, _2, _3, _4, _5, _6, name, ...) name
#define _dynarray_paste_pair(_1, _2) _1, _2
#define _dynarray_unfold_pair(_pair) _dynarray_paste_pair _pair

/**
 * there are 2 overloads for 3 passed functions: custom-move or custom-compare.
 * if move is defined, take default-compare.
 * if comp is defined, take fast-move.
 * otherwise, just define as usual.
 */

 #define _def_dynarray_3_fns_move(type, name, kind, fn) \
_def_dynarray_comp_fn_default(type, name) \
_def_dynarray_fn(name, kind, fn)

#define _def_dynarray_3_fns_comp(type, name, kind, fn) \
_def_dynarray_fn(name, kind, fn) \
_def_dynarray_internal_move_fast(type, name)

#define _def_dynarray_3_fns_copy(type, name, kind, fn) _def_dynarray_fn(name, kind, fn)

#define _def_dynarray_3_fns_free(type, name, kind, fn) _def_dynarray_fn(name, kind, fn)

#define _def_dynarray_3_fns_conditional(type, name, kind, fn) _def_dynarray_3_fns_##kind(type, name, kind, fn)

/* overloads for define_dynarray() */
/* regular pod-types with default-comparison. */
#define _def_dynarray_1(type, name) \
_def_dynarray_comp_fn_default(type, name) \
_def_dynarray_special_fns(type, name) \
_def_dynarray_modifier_fns(type, name) \
_def_dynarray_search_fns(type, name)

/* pod-types with custom-comparison. */
#define _def_dynarray_2(type, name, fn_1) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_1))) \
_def_dynarray_special_fns(type, name) \
_def_dynarray_modifier_fns(type, name) \
_def_dynarray_search_fns(type, name)

/* non-pod-types */
#define _def_dynarray_3(type, name, fn_1, fn_2) \
_def_dynarray_comp_fn_default(type, name) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_1))) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_2))) \
_def_dynarray_internal_copy(type, name) \
_def_dynarray_internal_move_fast(type, name) \
_def_dynarray_internal_free(type, name) \
_def_dynarray_special_fns_non_pod(type, name) \
_def_dynarray_modifier_fns_non_pod(type, name) \
_def_dynarray_search_fns(type, name)

/* non-pod-types */
#define _def_dynarray_4(type, name, fn_1, fn_2, fn_3) \
_def_dynarray_3_fns_conditional _dynarray_expand((type, name, _dynarray_unfold_pair(fn_1))) \
_def_dynarray_3_fns_conditional _dynarray_expand((type, name, _dynarray_unfold_pair(fn_2))) \
_def_dynarray_3_fns_conditional _dynarray_expand((type, name, _dynarray_unfold_pair(fn_3))) \
_def_dynarray_internal_copy(type, name) \
_def_dynarray_internal_free(type, name) \
_def_dynarray_special_fns_non_pod(type, name) \
_def_dynarray_modifier_fns_non_pod(type, name) \
_def_dynarray_search_fns(type, name)

/* non-pod-types with custom move and comparison. */
#define _def_dynarray_5(type, name, fn_1, fn_2, fn_3, fn_4) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_1))) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_2))) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_3))) \
_def_dynarray_fn _dynarray_expand((name, _dynarray_unfold_pair(fn_4))) \
_def_dynarray_internal_copy(type, name) \
_def_dynarray_internal_move(type, name) \
_def_dynarray_internal_free(type, name) \
_def_dynarray_special_fns_non_pod(type, name) \
_def_dynarray_modifier_fns_non_pod(type, name) \
_def_dynarray_search_fns(type, name)

/**
 * @brief defines all dynarray-functionality for a dynarray with <type> as it's value-type.
 *        preferably use this in your source-files.
 * @details supports 6 overloads:
 *        1.) regular pod-types with default-comparison.
 *          define_dynarray(type, name)
 *        2.) pod-types with custom-comparison.
 *          define_dynarray(type, name, (comp, &comp_fn))
 *        3.) non-pod-types with default-comparison.
 *          define_dynarray(type, name, (copy, &copy_fn), (free, &free_fn))
 *        4.) non-pod-types with custom-move.
 *          define_dynarray(type, name, (copy, &copy_fn), (free, &free_fn), (move, &move_fn))
 *        5.) non-pod-types with custom-comparison.
 *          define_dynarray(type, name, (copy, &copy_fn), (free, &free_fn), (comp, &comp_fn))
 *        6.) non-pod-types with custom-comparison and move.
 *          define_dynarray(type, name, (copy, &copy_fn), (free, &free_fn), (move, &move_fn), (comp, &comp_fn))
 * 
 *        the signatures for your passed predicates have to be:
 *        1.) copying: 
 *          dynarray_result_t copy(type* dest, const type* src);
 *        where a return-value of DYNARRAY_SUCCESS indicates a successful copy and anything else an error.
 *        2.) moving:
 *          void move(type* dest, type* src);
 *        3.) freeing:
 *          void free(type* ptr);
 *        4.) comparing:
 *          bool comp(const type* a, const type* b);
 *        where a return-value of true indicates equality.
 *
 *        passing custom functions in pairs with their name as in '(comp, &<comp_func>)'
 *        is mandatory when using non-pod types.
 */
#define define_dynarray(...) \
_dynarray_expand(_dynarray_select_macro(__VA_ARGS__, \
    _def_dynarray_5, \
    _def_dynarray_4, \
    _def_dynarray_3, \
    _def_dynarray_2, \
    _def_dynarray_1)(__VA_ARGS__))

/**
 * @}
 */

#ifdef BUILD_TESTS
int dynarray_test(FILE* fno);
#endif

#endif