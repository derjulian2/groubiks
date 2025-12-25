
#include <groubiks/utility/vector.h>

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

#ifdef BUILD_TESTS
declare_vector(int, int);
declare_vector_non_pod(char*, str);

define_vector(int, int);
define_vector_non_pod(char*, str);

bool vector_copy_value_fn(str)(char** dest, const char** src) {
    assert(dest && src);
    return (*dest = strdup(*src)) == NULL;
}

void vector_move_value_fn(str)(char** dest, char** src) {
    assert(dest && src);
    *dest = *src;
}

void vector_free_value_fn(str)(char** ptr) {
    assert(ptr);
    free(*ptr);
}

bool vector_compare_values_fn(str)(const char** a, const char** b) {
    assert(a && b);
    return strcmp(*a, *b) == 0;
}

int cmp(const int* a, const int* b) { 
    assert(a && b);
    if (*a < *b) { return -1; }
    if (*a > *b) { return 1; }
    return 0;
}

int vector_test(FILE* fno) {
    vector_result_t err = 0;

    /**
     * @name pod vector test: int
     * @{
     */

    int data[] = { 1, 2, 3, 4, 5 };

    vector_t(int) vec_1 = make_vector(int, &data[0], 5, &err);
    /* [ 1 2 3 4 5 ] */
    print_int_vec(stdout, vec_1);

    vector_insert(int, &vec_1, 2, 69, &err);
    vector_push_back(int, &vec_1, 420, &err);
    vector_insert_range(int, &vec_1, 0, &data[2], 2, &err);

    /* [ 3 4 1 2 69 3 4 5 420 ] */
    print_int_vec(stdout, vec_1);

    vector_t(int) uniq = vector_uniques(int, &vec_1);
    /* [ 3 4 1 2 69 5 420 ]*/
    print_int_vec(stdout, uniq);

    vector_erase(int, &vec_1, 2, &err);
    vector_erase_range(int, &vec_1, 1, 2, &err);

    /* [ 3 69 3 4 5 420 ] */
    print_int_vec(stdout, vec_1);

    fprintf(fno, "vector contains 420 : %d at %d\n", 
        vector_contains(int, &vec_1, 420),
        (int)vector_index(&vec_1, vector_find(int, &vec_1, 420)));
    fprintf(fno, "vector contains 512 : %d at %d\n", 
        vector_contains(int, &vec_1, 512),
        (int)vector_index(&vec_1, vector_find(int, &vec_1, 512)));

    vector_resize(int, &vec_1, 2, &err);

    /* [ 3 69 ] */
    print_int_vec(fno, vec_1);

    vector_resize(int, &vec_1, 4, &err);
    memset(vector_at(&vec_1, 2), 0, sizeof(int) * 2);

    /* [ 3 69 0 0 ]*/
    print_int_vec(fno, vec_1);

    free_vector(int, &uniq);
    free_vector(int, &vec_1);

    /**
     * @}
     */

    /**
     * @name non-pod vector test: strings
     * @{
     */

    const char* strings[] = { "iltam", "sumra", "rashupti" };

    vector_t(str) string_vec = make_vector(str, &strings[0], 3, &err);
    
    print_str_vec(stdout, string_vec);

    vector_push_back(str, &string_vec, "sus", &err);
    vector_push_back(str, &string_vec, "galileel", &err);

    print_str_vec(stdout, string_vec);
    vector_resize(str, &string_vec, 2, &err);
    print_str_vec(stdout, string_vec);
    vector_reserve(str, &string_vec, 20, &err);
    print_str_vec(stdout, string_vec);
    vector_shrink_to_fit(str, &string_vec, &err);
    print_str_vec(stdout, string_vec);

    free_vector(str, &string_vec);
    
    /**
     * @}
     */

    return err;
}
#endif