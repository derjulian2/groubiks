
#include <groubiks/utility/string.h>

string_t make_string(const char* str, string_result_t* err) {
    assert(str != NULL);
    assert(err != NULL);
    string_t tmp = {
        .data = strdup(str),
        .size = strlen(str) + 1
    };
    *err = tmp.data == NULL;
    return tmp;
}

string_t copy_string(string_t* str, string_result_t* err) {
    assert(str != NULL);
    assert(err != NULL);
    string_t tmp = {
        .data = malloc(sizeof(char) * str->size),
        .size = str->size
    };
    if ((*err = (tmp.data == NULL) != 0)) {
        return tmp;
    }
    memcpy(tmp.data, str->data, sizeof(char) * str->size);
    return tmp;
}

string_t* string_append(string_t* str1, string_t* str2, string_result_t* err) {
    assert(str1 != NULL);
    assert(str2 != NULL);
    assert(str1 != str2);
    char* ptr = realloc(str1->data, sizeof(char) * ((str1->size + str2->size) - 1));
    if ((*err = (ptr == NULL)) != 0) {
        return NULL;
    }
    str1->data = ptr;
    memcpy(&str1->data[str1->size - 1], str2->data, sizeof(char) * str2->size);
    str1->size = (str1->size + str2->size) - 1;
    return str1;
}

bool string_empty(string_t* str) {
    assert(str != NULL);
    return str->size - 1;
}

string_len_t string_len(string_t* str) {
    assert(str != NULL);
    return str->size - 1;
}

void free_string(string_t* str) {
    assert(str != NULL);
    free(str->data);
}

#ifdef BUILD_TESTS
int string_test(FILE* fno) {
    int err = 0;

    string_t string_1 = make_string("iltamsumra", &err);
    string_t string_2 = make_string("", &err);
    string_t string_3 = copy_string(&string_1, &err);

    fprintf(fno, "string_1 : length %d : %s\n", string_len(&string_1), string_1.data);
    fprintf(fno, "string_2 : length %d : %s\n", string_len(&string_2), string_2.data);
    fprintf(fno, "string_3 : length %d : %s\n", string_len(&string_3), string_3.data);

    string_append(&string_1, &string_3, &err);
    fprintf(fno, "string_1 : length %d : %s\n", string_len(&string_1), string_1.data);

    free_string(&string_1);
    free_string(&string_2);
    free_string(&string_3);

    return err;
}
#endif