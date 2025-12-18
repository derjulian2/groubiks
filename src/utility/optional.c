
#include <groubiks/utility/optional.h>

#ifdef BUILD_TESTS

declare_optional(int);

int optional_test(FILE* fno) {
    
    optional_t(int) i = nullopt(int);

    assert(!i.has_value); 
    fputs("i does not hold a value\n", fno);

    i = make_optional(int, 69420);

    assert(i.has_value);
    if (i.has_value)
    { fprintf(fno, "i now has a value of %d\n", i.value); }

    return 0;
}

#endif