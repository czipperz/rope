#include "test.h"

#include <stdio.h>

int failures = 0;
int successes = 0;
int successes_assert = 0;

#define run(test)                                                    \
    do {                                                             \
        void test();                                                 \
        test();                                                      \
    } while (0)

int main() {
    run(test_rope);
    printf("%d of %d succeeded.\n", successes, failures + successes);
    printf("%d assertions succeeded.\n", successes_assert);
    return failures;
}
