#include <bits.hpp>
#include <cmath>
#include <gtest/gtest.h>
#include <test_tools.hpp>

namespace {

using slice = test_base;

TEST_F(slice, create) {
    (void)bits{ivalue}(0, 32);
    (void)bits{fvalue}(0, 1);
}

TEST_F(slice, first) {
    EXPECT_EQ(bits{ivalue}(15, 32).first(), 15);
    EXPECT_EQ(bits{fvalue}(0, 10).first(), 0);
}

TEST_F(slice, last) {
    EXPECT_EQ(bits{ivalue}(5, 10).last(), 10);
    EXPECT_EQ(bits{fvalue}(0, 1).last(), 1);
}

TEST_F(slice, assigment) {
    bits{ivalue}(0, 32) = 10;
    EXPECT_EQ(ivalue, 10);
    bits{ivalue}(0, 0) = 5;
    EXPECT_EQ(ivalue, 10);
    bits{ivalue}(32, 32) = 7;
    EXPECT_EQ(ivalue, 10);
    bits{ivalue}(0, 4) = 0x1E;
    EXPECT_EQ(ivalue, 14);
    bits{ivalue}(0, 3) = 0b0000'0111;
    EXPECT_EQ(ivalue, 15);

    bits{fvalue}(0, 23) = 0b1111'10110100110001111100011;
    EXPECT_EQ(fvalue, 1.70617330074f);
    bits{fvalue}(0, 32) = 0;
    EXPECT_EQ(fvalue, 0.f);
    bits{fvalue}(31, 32) = 1;
    EXPECT_EQ(fvalue, -0.f);
    bits{fvalue}(23, 31) = 0b111111111;
    EXPECT_TRUE(std::isinf(fvalue));
    EXPECT_EQ(bits{fvalue}, 0xFF800000);
}

} // namespace
