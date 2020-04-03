#include "mm7-core.h"

/**
 * Entry points for tests.
 */

#define TRUE_OR_RETURN(cond) if(!(cond)) return 0;

static int test_order_init(void)
{
    mm7_Order foo;
    mm7_Order_init(&foo, 3, 10.0, 2, 20.0);
    TRUE_OR_RETURN(foo.selling.id == 3);
    TRUE_OR_RETURN(foo.buying.id == 2);
    TRUE_OR_RETURN(foo.exrate == 0.5);
    return 1;
}
 

struct mm7_test {
    const char* name;
    int (*test)(void);
};

static const 
struct mm7_test TESTING_MAP[] = {
    {"order_init", &test_order_init}
};

static const 
size_t TEST_COUNT = sizeof(TESTING_MAP) / sizeof(struct mm7_test);


static void __mm7_run_test(const struct mm7_test* test)
{
    int result = test->test();
    printf("TEST %s RESULT %s\n", test->name, result ? "PASS" : "FAIL");
}

static const struct mm7_test* 
__mm7_find_test(const char* name)
{
    const struct mm7_test* ptr = TESTING_MAP;
    size_t testCount = TEST_COUNT;
    while (testCount--) {
        if (strcmp(ptr->name, name) == 0)
            return ptr;
        ++ptr;
    }
    return NULL;
}

#undef TRUE_OR_RETURN

int main(int argc, char const* argv[])
{
    if (argc < 2) {
        fprintf(stderr,  "Error: %s\n", "expected tests to be passed as options.");
        exit(1);
    }
    const char** begPtr = argv + 1;
    const char** endPtr = begPtr + (argc - 1);
    while (begPtr != endPtr) {
        const struct mm7_test* found = __mm7_find_test(*begPtr);
        if(found == NULL) {
            fprintf(stderr, "Invalid Test Name:%s\n", *begPtr);
            exit(2);
        }
        __mm7_run_test(found);
        ++begPtr;
    }
    return 0;
}