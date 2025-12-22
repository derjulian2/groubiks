
#include <groubiks/utility/common.h>

define_vector(cstring_t);
define_vector(const_cstring_t);
define_vector(uint32_t);

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