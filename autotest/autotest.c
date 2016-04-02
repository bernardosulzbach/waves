#include "unity.h"

#include "geometry.h"

void test_minimum_works_as_expected() {
    TEST_ASSERT(minimum(-1.0, -1.0) == -1.0);

    TEST_ASSERT(minimum(-1.0, 0.0) == -1.0);
    TEST_ASSERT(minimum(0.0, -1.0) == -1.0);

    TEST_ASSERT(minimum(0.0, 0.0) == 0.0);

    TEST_ASSERT(minimum(0.0, 1.0) == 0.0);
    TEST_ASSERT(minimum(1.0, 0.0) == 0.0);

    TEST_ASSERT(minimum(1.0, 1.0) == 1.0);
}

void test_maximum_works_as_expected() {
    TEST_ASSERT(maximum(-1.0, -1.0) == -1.0);

    TEST_ASSERT(maximum(-1.0, 0.0) == 0.0);
    TEST_ASSERT(maximum(0.0, -1.0) == 0.0);

    TEST_ASSERT(maximum(0.0, 0.0) == 0.0);

    TEST_ASSERT(maximum(0.0, 1.0) == 1.0);
    TEST_ASSERT(maximum(1.0, 0.0) == 1.0);

    TEST_ASSERT(maximum(1.0, 1.0) == 1.0);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_minimum_works_as_expected);
    RUN_TEST(test_maximum_works_as_expected);
    return UNITY_END();
}
