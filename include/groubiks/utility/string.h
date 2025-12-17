
#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef BUILD_TESTS
#include <stdio.h>
#endif

#define empty_string (string_t){ .data = NULL, .size = -1ull }

typedef int string_result_t;
typedef int string_len_t;

typedef struct {
    char* data;
    size_t size;
} string_t;

string_t make_string(const char* str, string_result_t* err);
string_t copy_string(string_t* str, string_result_t* err);

string_t* string_append(string_t* str1, string_t* str2, string_result_t* err);

bool string_empty(string_t* str);
string_len_t string_len(string_t* str);

void free_string(string_t* str);

#ifdef BUILD_TESTS
int string_test(FILE* fno);
#endif

#endif