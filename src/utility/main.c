
#include <groubiks/utility/dynarray.h>
#include <groubiks/utility/optional.h>

#ifdef BUILD_TESTS
int main(int argc, char** argv) {
    return dynarray_test(stdout) || optional_test(stdout);
}
#endif