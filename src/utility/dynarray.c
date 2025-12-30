
#include <groubiks/utility/dynarray.h>

#ifdef BUILD_TESTS

/**
 * @file dynarray.c
 * @date 29/12/25
 * @author Julian Benzel
 * @brief dynarray.h unit-test.
 */

/* print-utilites */
#define print_int_dyn(fno, dyn) \
fprintf(fno, #dyn" capacity: %lu, size: %lu, [ ", dyn.capacity, dyn.size); \
for (int i = 0; i < dyn.size; ++i) { \
    fprintf(fno, "%d ", *dynarray_at(&dyn, i)); \
} \
fprintf(fno, "]\n");

#define print_str_dyn(fno, dyn) \
fprintf(fno, #dyn" capacity: %lu, size: %lu, [ ", dyn.capacity, dyn.size); \
for (int i = 0; i < dyn.size; ++i) { \
    fprintf(fno, "%s ", *dynarray_at(&dyn, i)); \
} \
fprintf(fno, "]\n");

/**
 * @brief integer-dynarray, declaration and definition.
 */
declare_dynarray(int, i32);
define_dynarray(int, i32);

/**
 * @brief string-dynarray, declaration and definition with special functions.
 */

dynarray_result_t _copy_cstr(char** dest, const char** src) {
    assert(dest && src);
    return (*dest = strdup(*src)) == NULL ? DYNARRAY_ERROR : DYNARRAY_SUCCESS;
}

void _free_cstr(char** ptr) {
    assert(ptr);
    free(*ptr);
}

bool _comp_cstr(const char** a, const char** b) {
    assert(a && b);
    return strcmp(*a, *b) == 0;
}

/* all functions passed to the dynarray. */
declare_dynarray(char*, cstr);
define_dynarray(char*, cstr,
    (copy, &_copy_cstr),
    (free, &_free_cstr),
    (comp, &_comp_cstr)
);

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

int dynarray_test(FILE* fno) {
    dynarray_result_t err = 0;

    /**
     * @name pod dynarray test: int
     * @{
     */

    int data[] = { 1, 2, 3, 4, 5 };

    dynarray_t(i32) dyn_1 = make_dynarray(i32, &data[0], 5, &err);
    /* [ 1 2 3 4 5 ] */
    print_int_dyn(stdout, dyn_1);

    dynarray_insert(i32, &dyn_1, 2, 69, &err);
    dynarray_push_back(i32, &dyn_1, 420, &err);
    dynarray_insert_range(i32, &dyn_1, 0, &data[2], 2, &err);

    /* [ 3 4 1 2 69 3 4 5 420 ] */
    print_int_dyn(stdout, dyn_1);

    dynarray_t(i32) uniq = dynarray_uniques(i32, &dyn_1);
    /* [ 3 4 1 2 69 5 420 ]*/
    print_int_dyn(stdout, uniq);

    dynarray_erase(i32, &dyn_1, 2, &err);
    dynarray_erase_range(i32, &dyn_1, 1, 2, &err);

    /* [ 3 69 3 4 5 420 ] */
    print_int_dyn(stdout, dyn_1);

    fprintf(fno, "dynarray contains 420 : %d at %d\n", 
        dynarray_contains(i32, &dyn_1, 420),
        (int)dynarray_index(&dyn_1, dynarray_find(i32, &dyn_1, 420)));
    fprintf(fno, "dynarray contains 512 : %d at %d\n", 
        dynarray_contains(i32, &dyn_1, 512),
        (int)dynarray_index(&dyn_1, dynarray_find(i32, &dyn_1, 512)));

    dynarray_resize(i32, &dyn_1, 2, &err);

    /* [ 3 69 ] */
    print_int_dyn(fno, dyn_1);

    dynarray_resize(i32, &dyn_1, 4, &err);
    memset(dynarray_at(&dyn_1, 2), 0, sizeof(int) * 2);

    /* [ 3 69 0 0 ]*/
    print_int_dyn(fno, dyn_1);

    free_dynarray(i32, &uniq);
    free_dynarray(i32, &dyn_1);

    /**
     * @}
     */

    /**
     * @name non-pod dynarray test: strings
     * @{
     */
    const char* strings[] = { "iltam", "sumra", "rashupti" };

    dynarray_t(cstr) string_dyn = make_dynarray(cstr, &strings[0], 3, &err);
    
    print_str_dyn(stdout, string_dyn);

    dynarray_push_back(cstr, &string_dyn, "sus", &err);
    dynarray_push_back(cstr, &string_dyn, "galileel", &err);

    print_str_dyn(stdout, string_dyn);
    dynarray_resize(cstr, &string_dyn, 2, &err);
    print_str_dyn(stdout, string_dyn);
    dynarray_reserve(cstr, &string_dyn, 20, &err);
    print_str_dyn(stdout, string_dyn);
    dynarray_shrink_to_fit(cstr, &string_dyn, &err);
    print_str_dyn(stdout, string_dyn);

    free_dynarray(cstr, &string_dyn);
    
    /**
     * @}
     */

    return err;
}
#endif