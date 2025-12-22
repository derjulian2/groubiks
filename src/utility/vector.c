
#include <groubiks/utility/vector.h>

#define print_int_vec(fno, vec) \
fprintf(fno, #vec" capacity: %lu, size: %lu, [ ", vec.capacity, vec.size); \
for (int i = 0; i < vec.size; ++i) { \
    fprintf(fno, "%d ", *vector_at(&vec, i)); \
} \
fprintf(fno, "]\n");

#ifdef BUILD_TESTS
declare_vector(int);

define_vector(int);
define_vector_default_compare_predicate(int);

int vector_test(FILE* fno) {
    vector_result_t err = 0;

    int i = 202;

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
        (int)vector_make_index(&vec_1, vector_find(int, &vec_1, 420)));
    fprintf(fno, "vector contains 512 : %d at %d\n", 
        vector_contains(int, &vec_1, 512),
        (int)vector_make_index(&vec_1, vector_find(int, &vec_1, 512)));

    vector_resize(int, &vec_1, 2, &err);

    /* [ 3 69 ] */
    print_int_vec(fno, vec_1);

    vector_resize(int, &vec_1, 4, &err);
    memset(vector_at(&vec_1, 2), 0, sizeof(int) * 2);

    /* [ 3 69 0 0 ]*/
    print_int_vec(fno, vec_1);

    free_vector(&uniq);
    free_vector(&vec_1);

    return err;
}
#endif