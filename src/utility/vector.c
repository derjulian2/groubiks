
#include <groubiks/utility/vector.h>

#ifdef BUILD_TESTS

/**
 * @file vector.c
 * @date 29/12/25
 * @author Julian Benzel
 * @brief vector.h unit-test.
 */

/* print-utilites */
#define print_int_vec(fno, vec) \
fprintf(fno, #vec" capacity: %lu, size: %lu, [ ", vec.capacity, vec.size); \
for (int i = 0; i < vec.size; ++i) { \
    fprintf(fno, "%d ", *vector_at(&vec, i)); \
} \
fprintf(fno, "]\n");

#define print_str_vec(fno, vec) \
fprintf(fno, #vec" capacity: %lu, size: %lu, [ ", vec.capacity, vec.size); \
for (int i = 0; i < vec.size; ++i) { \
    fprintf(fno, "%s ", *vector_at(&vec, i)); \
} \
fprintf(fno, "]\n");

/**
 * @brief integer-vector, declaration and definition.
 */
declare_vector(int, i32);
define_vector(int, i32);

/**
 * @brief string-vector, declaration and definition with special functions.
 */

vector_result_t _copy_cstr(char** dest, const char** src) {
    assert(dest && src);
    return (*dest = strdup(*src)) == NULL ? VECTOR_ERROR : VECTOR_SUCCESS;
}

void _move_cstr(char** dest, char** src) {
    assert(dest && src);
    *dest = *src;
}

void _free_cstr(char** ptr) {
    assert(ptr);
    free(*ptr);
}

bool _comp_cstr(const char** a, const char** b) {
    assert(a && b);
    return strcmp(*a, *b) == 0;
}

/* all functions passed to the vector. */
declare_vector(char*, cstr);
define_vector(char*, cstr, &_copy_cstr, &_move_cstr, &_free_cstr, &_comp_cstr);

/* predicates for qsort. */
int cmpi(const int* a, const int* b) { 
    assert(a && b);
    if (*a < *b) { return -1; }
    else if (*a > *b) { return 1; }
    return 0;
}

int cmps(const char** a, const char** b) {
    assert(a && b);
    size_t len_a = strlen(*a);
    size_t len_b = strlen(*b);
    if (len_a < len_b) { return -1; }
    else if (len_a > len_b) { return 1; }
    return 0;
}

int vector_test(FILE* fno) {
    vector_result_t err = 0;

    /**
     * @name pod vector test: int
     * @{
     */

    int data[] = { 1, 2, 3, 4, 5 };

    vector_t(i32) vec_1 = make_vector(i32, &data[0], 5, &err);
    /* [ 1 2 3 4 5 ] */
    print_int_vec(stdout, vec_1);

    vector_insert(i32, &vec_1, 2, 69, &err);
    vector_push_back(i32, &vec_1, 420, &err);
    vector_insert_range(i32, &vec_1, 0, &data[2], 2, &err);

    /* [ 3 4 1 2 69 3 4 5 420 ] */
    print_int_vec(stdout, vec_1);

    vector_t(i32) uniq = vector_uniques(i32, &vec_1);
    /* [ 3 4 1 2 69 5 420 ]*/
    print_int_vec(stdout, uniq);

    vector_erase(i32, &vec_1, 2, &err);
    vector_erase_range(i32, &vec_1, 1, 2, &err);

    /* [ 3 69 3 4 5 420 ] */
    print_int_vec(stdout, vec_1);

    fprintf(fno, "vector contains 420 : %d at %d\n", 
        vector_contains(i32, &vec_1, 420),
        (int)vector_index(&vec_1, vector_find(i32, &vec_1, 420)));
    fprintf(fno, "vector contains 512 : %d at %d\n", 
        vector_contains(i32, &vec_1, 512),
        (int)vector_index(&vec_1, vector_find(i32, &vec_1, 512)));

    vector_resize(i32, &vec_1, 2, &err);

    /* [ 3 69 ] */
    print_int_vec(fno, vec_1);

    vector_resize(i32, &vec_1, 4, &err);
    memset(vector_at(&vec_1, 2), 0, sizeof(int) * 2);

    /* [ 3 69 0 0 ]*/
    print_int_vec(fno, vec_1);

    free_vector(i32, &uniq);
    free_vector(i32, &vec_1);

    /**
     * @}
     */

    /**
     * @name non-pod vector test: strings
     * @{
     */
    const char* strings[] = { "iltam", "sumra", "rashupti" };

    vector_t(cstr) string_vec = make_vector(cstr, &strings[0], 3, &err);
    
    print_str_vec(stdout, string_vec);

    vector_push_back(cstr, &string_vec, "sus", &err);
    vector_push_back(cstr, &string_vec, "galileel", &err);

    print_str_vec(stdout, string_vec);
    vector_resize(cstr, &string_vec, 2, &err);
    print_str_vec(stdout, string_vec);
    vector_reserve(cstr, &string_vec, 20, &err);
    print_str_vec(stdout, string_vec);
    vector_shrink_to_fit(cstr, &string_vec, &err);
    print_str_vec(stdout, string_vec);

    free_vector(cstr, &string_vec);
    
    /**
     * @}
     */

    return err;
}
#endif