
#include <groubiks/utility/vector.h>

vector_t _make_vector(void* data, size_t num, size_t size, vector_result_t* err) {
    vector_t tmp = {
        .data = malloc(size * num),
        .size = num
    };
    if ((*err = (tmp.data == NULL)) != 0) {
        return tmp;
    }
    if (data != NULL) {
        memcpy(tmp.data, data, num * size);
    }
    return tmp;
}

void* _vector_insert(vector_t* vec, size_t num, size_t size, size_t idx, void* val, vector_result_t* err) {
    assert(vec != NULL);
    void* ptr = realloc(vec->data, size * (vec->size + num));
    if ((*err = (ptr == NULL)) != 0) {
        return NULL;
    }
    /* move all old elements after the to-be-inserted-range over */
    memmove(ptr + size * (idx + num + 1), 
            ptr + size * (idx + 1), 
            size * (vec->size - (idx + 1)));
    /* copy the to-be-inserted-range over */
    memcpy(ptr + size * (idx + 1), 
           val, 
           size * num);
    /* update */
    vec->data = ptr;
    vec->size = vec->size + num;
    return vec->data + size * idx;
}

void* _vector_erase(vector_t* vec, size_t num, size_t size, vector_index_t idx, vector_result_t* err) {
    assert(vec != NULL);
    assert(num <= vec->size - idx);
    void* ptr = malloc(size * (vec->size - num));
    if ((*err = (ptr == NULL)) != 0) {
        return NULL;
    }
    /* copy all elements until the to-be-deleted-range over */
    memcpy(ptr, 
           vec->data, 
           size * idx);
    /* overwrite everything else with all elements behind the to-be-deleted-range */
    memcpy(ptr + size * idx, 
           vec->data + size * (idx + num), 
           size * (vec->size - num - idx));
    free(vec->data);
    vec->data = ptr;
    vec->size = vec->size - num;
    return vec->data + idx * size;
}

void free_vector(vector_t* vec) {
    assert(vec != NULL);
    free(vec->data);
}

#define print_int_vec(fno, vec) \
for (int i = 0; i < vec.size; ++i) { \
    fprintf(fno, "%d ", *vector_at(int, &vec, i)); \
}

#ifdef BUILD_TESTS
int vector_test(FILE* fno) {
    int err = 0;
    int data[] = { 1, 2, 3, 4, 5 };

    vector_t vec_1 = make_vector(int, &data[0], 5, &err);

    /* [ 1 2 3 4 5 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_insert_range(int, &vec_1, 3, 3, &data[1], &err);

    /* [ 1 2 3 4 2 3 4 5 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_push_back(int, &vec_1, &data[0], &err);

    /* [ 1 2 3 4 2 3 4 5 1 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_erase(int, &vec_1, 3, &err);

    /* [ 1 2 3 2 3 4 5 1 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_erase_range(int, &vec_1, 3, 1, &err);

    /* [ 1 3 4 5 1 ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    vector_erase_range(int, &vec_1, 5, 0, &err);

    /* [ ] */
    fprintf(fno, "vec1: [ ");
    print_int_vec(stdout, vec_1);
    fprintf(fno, "]\n");

    free_vector(&vec_1);

    return err;
}
#endif