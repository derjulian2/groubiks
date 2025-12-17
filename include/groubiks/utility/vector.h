
#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

/**
 * @file vector.h
 * @date 17/12/2025
 * @author Julian Benzel
 * @brief type-generic dynamically-sized array data-structure based on void*.
 *        be VERY careful to use the correct type-casts when using this.
 *        i should probably go back to implementing this via macro-hell for type-safety.
 *        also i should do it like std::vector and include a capacity that gets doubled
 *        each time it is reached.
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#ifdef BUILD_TESTS
#include <stdio.h>
#endif

#define null_vector (vector_t){ .data = NULL, .size = -1ull }

typedef int vector_result_t;
typedef size_t vector_index_t;

typedef struct {
    void* data;
    size_t size;
} vector_t;

vector_t _make_vector(void* data, size_t num, size_t size, vector_result_t* err);

void* _vector_insert(vector_t* vec, size_t num, size_t size, size_t idx, void* val, vector_result_t* err);
void* _vector_erase(vector_t* vec, size_t num, size_t size, vector_index_t idx, vector_result_t* err);

void free_vector(vector_t* vec);

#define make_vector(type, data, n, err) _make_vector(data, n, sizeof(type), err)
#define vector_at(type, vec, i) (((type*)(vec)->data) + i)
#define vector_insert_range(type, vec, n, i, val, err) (type*) _vector_insert(vec, n, sizeof(type), i, val, err)
#define vector_insert(type, vec, i, val, err) (type*) _vector_insert(vec, 1, sizeof(type), i, val, err)
#define vector_push_back(type, vec, val, err) (type*) _vector_insert(vec, 1, sizeof(type), (*(vec)).size - 1, val, err)
#define vector_erase_range(type, vec, n , i, err) (type*) _vector_erase(vec, n, sizeof(type), i, err)
#define vector_erase(type, vec, i, err) (type*) _vector_erase(vec, 1, sizeof(type), i, err)
#define vector_back(type, vec) (((type*)(vec)->data) + vec->size - 1)
#define vector_front(type, vec) ((type*)(vec)->data)

#ifdef BUILD_TESTS
int vector_test(FILE* fno);
#endif

#endif