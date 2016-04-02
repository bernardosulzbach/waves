#include "unity.h"

#include "constants.h"
#include "waves.h"

void test_lookup_sin_is_precise_enough() {
    for (int i = -10; i < 10; i++) {
        for (int j = 
    for (double arg = - 4 * TAU; arg < 4 * TAU; arg += 0.05) {
        TEST_ASSERT(fabs(lookup_sin(arg) - sin(arg)) < 0.1);
    }
}

int main() {
    UNITY_BEGIN();
    init_sin_table();
    RUN_TEST(test_lookup_sin_is_precise_enough);
    return UNITY_END();
}
