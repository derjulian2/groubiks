
#include <groubiks/utility/vector.h>
#include <groubiks/utility/string.h>

#ifdef BUILD_TESTS
int main(int argc, char** argv) {
    return vector_test(stdout) || string_test(stdout);
}
#endif