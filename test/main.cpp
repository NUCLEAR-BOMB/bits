#include <array>
#include <gtest/gtest.h>
#include <string>
#include <vector>

int main(int argc, char** argv) {
#ifdef ENABLE_GTEST_BRIEF
    ::testing::FLAGS_gtest_brief = true;
#endif
#ifdef ENABLE_GTEST_DEBUG_BREAK
    ::testing::FLAGS_gtest_catch_exceptions = false;
    ::testing::FLAGS_gtest_break_on_failure = true;
#endif

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
