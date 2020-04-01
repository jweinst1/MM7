#include "mm7-core.h"

/**
 * Entry points for tests.
 */
 

struct mm7_test {
    const char* name;
    int (*test)(void);
};

struct mm7_test TESTING_MAP[] = {
    {"order_init", NULL}
};

size_t TEST_COUNT = sizeof(TESTING_MAP) / sizeof(mm7_test);


static void __mm7_run_test(const struct mm7_test* test)
{
    int result = test->test();
    printf("TEST %s RESULT %s\n", test->name, result ? "PASS" : "FAIL");
}

int main(int argc, char const* argv[])
{
    
    return 0;
}