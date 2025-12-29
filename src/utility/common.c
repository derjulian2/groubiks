
#include <groubiks/utility/common.h>

define_vector(uint32_t, u32);

vector_result_t _copy_str(char** dest, const char** src) {
    assert(dest && src);
    return (*dest = strdup(*src)) == NULL ? VECTOR_ERROR : VECTOR_SUCCESS;
}

void _move_str(char** dest, char** src) {
    assert(dest && src);
    *dest = *src;
}

void _free_str(char** ptr) {
    assert(ptr);
    free(*ptr);
}

bool _comp_str(const char** a, const char** b) {
    assert(a && b);
    return strcmp(*a, *b) == 0;
}

define_vector(char*, str, &_copy_str, &_move_str, &_free_str, &_comp_str);

int clampi(int v, int lo, int hi) {
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

unsigned int clampui(unsigned int v, unsigned int lo, unsigned int hi) {
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

int readFile(const char* path, char** ptr, size_t* size) {
    assert(path != NULL && ptr != NULL);
    FILE* fstream = fopen(path, "rb");
    if (fstream == NULL)
    { goto error; }
    fseek(fstream, 0, SEEK_END);
    size_t fsize = ftell(fstream);
    fseek(fstream, 0, SEEK_SET);
    char* fdata = malloc(sizeof(char) * fsize);
    if (fdata == NULL)
    { goto error; }
    if (fread(fdata, sizeof(char), fsize, fstream) != fsize)
    { goto error; }
    *ptr = fdata;
    *size = fsize;
    fclose(fstream);
    return 0;
error:
    free(fdata);
    if (fstream != NULL)
    { fclose(fstream); }
    return -1;
}