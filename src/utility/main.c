
#include <groubiks/utility/vector.h>
#include <groubiks/utility/optional.h>

#ifdef BUILD_TESTS
int main(int argc, char** argv) {
    return vector_test(stdout) || optional_test(stdout);
}
#endif