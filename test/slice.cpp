#include <bits.hpp>
#include <gtest/gtest.h>

namespace {

TEST(slice, constructor) {
    (void)bits<>::slice(1, 2);
    (void)bits<>::slice({}, 2);
    (void)bits<>::slice(3, {});
}

TEST(slice, start_end) {
    const auto s1 = bits<>::slice(1, 2);
    EXPECT_EQ(s1.start(), 1);
    EXPECT_EQ(s1.end(), 2);

    const auto s2 = bits<>::slice({}, 3);
    EXPECT_EQ(s2.start(), 0);
    EXPECT_EQ(s2.end(), 3);

    const bits<>::slice s3(2, {});
    EXPECT_EQ(s3.start(), 2);
    EXPECT_EQ(s3.end(), -1);

    EXPECT_EQ(bits<>::slice(1, 2).start(), 1);
    EXPECT_EQ(bits<>::slice({}, 2).end(), 2);
}

} // namespace