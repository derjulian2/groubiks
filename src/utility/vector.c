
#include <groubiks/utility/vector.h>

declare_vector(int)

define_vector(int)


#define print_int_vec(fno, vec) \
for (int i = 0; i < vec.size; ++i) { \
    fprintf(fno, "%d ", *vector_at(int, &vec, i)); \
}

#ifdef BUILD_TESTS
int vector_test(FILE* fno) {
    int err = 0;
    int data[] = { 1, 2, 3, 4, 5 };

    vector_t(int) vec_1 = make_vector(int, &data[0], 5, &err);
    /* [ 1 2 3 4 5 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_insert(int, &vec_1, 2, 69, &err);
    vector_push_back(int, &vec_1, 420, &err);
    vector_insert_range(int, &vec_1, 0, &data[2], 2, &err);

    /* [ 3 4 1 2 69 3 4 5 420 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_erase(int, &vec_1, 2, &err);
    vector_erase_range(int, &vec_1, 1, 2, &err);

    /* [ 3 69 3 4 5 420 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    free_vector(int, &vec_1);

    return err;
}
#endif