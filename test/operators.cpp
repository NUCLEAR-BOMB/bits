#include <bits.hpp>
#include <cmath>
#include <cstdint>
#include <gtest/gtest.h>
#include <test_tools.hpp>

// for floats https://www.h-schmidt.net/FloatConverter/IEEE754.html

namespace {

using operators = test_struct;

TEST_F(operators, assigment) {
    bits{ivalue} = 2;
    EXPECT_EQ(ivalue, 2);
}

TEST_F(operators, narrow_assigment) {
    bits{ivalue}.narrow() = 2;
    EXPECT_EQ(ivalue, 2);
    bits{ivalue}.narrow() = 10LL;
    EXPECT_EQ(ivalue, 10);
    bits{ivalue}.narrow() = short(255);
    EXPECT_EQ(ivalue, 255);

    constexpr bool compile_time_narrow_assigment = [] {
        int val = 0;
        bits{val}.narrow() = 2;
        if (val != 2) return false;
        bits{val}.narrow() = 100'000LL;
        if (val != 100'000) return false;
        bits{val}.narrow() = short(-1);
        if (val != -1) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_narrow_assigment);
}

TEST_F(operators, equal) {
    EXPECT_TRUE(bits{ivalue} == 1);
    EXPECT_TRUE(bits{const_ivalue} == 2);
    EXPECT_TRUE(1 == bits{ivalue});
    EXPECT_TRUE(2 == bits{const_ivalue});
    EXPECT_TRUE(bits{ivalue} == bits{ivalue});
    EXPECT_TRUE(bits{const_ivalue} == bits{const_ivalue});

    EXPECT_FALSE(bits{ivalue} == 2);
    EXPECT_FALSE(2 == bits{ivalue});
    EXPECT_FALSE(bits{ivalue} == bits{2});
    EXPECT_FALSE(bits{2} == bits{ivalue});

    constexpr bool compile_time_equal = [] {
        int val = 2;
        if (!(bits{val} == 2)) return false;
        if (!(2 == bits{val})) return false;
        if (!(bits{val} == bits{val})) return false;
        if (!(bits{val} == bits{2})) return false;
        if (!(bits{2} == bits{val})) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_equal);

    const std::array<int, 4> arr1{1, 0};
    const std::array<int, 4> arr2{1, 0};
    EXPECT_TRUE(bits{arr1} == bits{arr2});
}

TEST_F(operators, not_equal) {
    EXPECT_FALSE(bits{ivalue} != 1);
    EXPECT_FALSE(bits{const_ivalue} != 2);
    EXPECT_FALSE(1 != bits{ivalue});
    EXPECT_FALSE(2 != bits{const_ivalue});
    EXPECT_FALSE(bits{ivalue} != bits{ivalue});
    EXPECT_FALSE(bits{const_ivalue} != bits{const_ivalue});

    EXPECT_TRUE(bits{ivalue} != 2);
    EXPECT_TRUE(2 != bits{ivalue});
    EXPECT_TRUE(bits{ivalue} != bits{2});
    EXPECT_TRUE(bits{2} != bits{ivalue});

    constexpr bool compile_time_not_equal = [] {
        int val = -1;
        if (bits{val} != -1) return false;
        if (-1 != bits{val}) return false;
        if (bits{val} != bits{val}) return false;
        if (bits{val} != bits{-1}) return false;
        if (bits{-1} != bits{val}) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_not_equal);

    const std::array<int, 4> arr1{10, 20, 30};
    const std::array<int, 4> arr2{30, 20, 10};
    EXPECT_TRUE(bits{arr1} != bits{arr2});
}

TEST_F(operators, greater) {
    EXPECT_TRUE(bits{ivalue} > 0);
    EXPECT_TRUE(bits{const_ivalue} > 1);
    EXPECT_FALSE(0 > bits{ivalue});
    EXPECT_FALSE(1 > bits{const_ivalue});
}

TEST_F(operators, greater_equal) {
    EXPECT_TRUE(bits{ivalue} >= 1);
    EXPECT_TRUE(bits{const_ivalue} >= 2);
    EXPECT_FALSE(0 >= bits{ivalue});
    EXPECT_FALSE(1 >= bits{const_ivalue});
}

TEST_F(operators, less) {
    EXPECT_TRUE(bits{ivalue} < 2);
    EXPECT_TRUE(bits{const_ivalue} < 3);
    EXPECT_FALSE(1 < bits{ivalue});
    EXPECT_FALSE(2 < bits{const_ivalue});
}

TEST_F(operators, less_equal) {
    EXPECT_TRUE(bits{ivalue} <= 1);
    EXPECT_TRUE(bits{const_ivalue} <= 2);
    EXPECT_FALSE(2 <= bits{ivalue});
    EXPECT_FALSE(3 <= bits{const_ivalue});
}

TEST_F(operators, addition_assignment) {
    bits{ivalue} += 1;
    EXPECT_EQ(ivalue, 2);
    bits{ivalue} += 100u;
    EXPECT_EQ(ivalue, 102);
    bits{ivalue} += unsigned(-2);
    EXPECT_EQ(ivalue, 100);
    bits{ivalue} += unsigned(-100);
    EXPECT_EQ(ivalue, 0);

    constexpr bool compile_time_add = [] {
        int val = 1;
        bits{val} += 2;
        if (val != 3) return false;
        bits{val} += unsigned(-2);
        if (val != 1) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_add);

    bits{fvalue} += 1 << 23;
    EXPECT_EQ(fvalue, 2.f);
    bits{fvalue} += 1 << 22;
    EXPECT_EQ(fvalue, 3.f);
}

TEST_F(operators, subtraction_assignment) {
    bits{ivalue} -= 1;
    EXPECT_EQ(ivalue, 0);
    bits{ivalue} -= 100u;
    EXPECT_EQ(ivalue, -100);
    bits{ivalue} -= unsigned(-10);
    EXPECT_EQ(ivalue, -90);
    bits{ivalue} -= unsigned(-100);
    EXPECT_EQ(ivalue, 10);

    constexpr bool compile_time_sub = [] {
        int val = 6;
        bits{val} -= 2;
        if (val != 4) return false;
        bits{val} -= 101;
        if (val != -97) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_sub);

    bits{fvalue} -= 1 << 22;
    EXPECT_EQ(fvalue, 0.75f);
    bits{fvalue} -= 1u << 30;
    EXPECT_EQ(bits{fvalue}, 0xFF400000);
}

TEST_F(operators, multiplication_assignment) {
    bits{ivalue} *= 2;
    EXPECT_EQ(ivalue, 2);
    bits{ivalue} *= 5u;
    EXPECT_EQ(ivalue, 10);
    bits{ivalue} *= 0;
    EXPECT_EQ(ivalue, 0);

    constexpr bool compile_time_mul = [] {
        int val = 10;
        bits{val} *= 1;
        if (val != 10) return false;
        bits{val} *= 2;
        if (val != 20) return false;
        bits{val} *= 0;
        if (val != 0) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_mul);
}

TEST_F(operators, division_assignment) {
    bits{ivalue} /= 2;
    EXPECT_EQ(ivalue, 0);
    bits{ivalue} = 25;
    bits{ivalue} /= 5;
    EXPECT_EQ(ivalue, 5);

    constexpr bool compile_time_div = [] {
        int val = 124;
        bits{val} /= 2;
        if (val != 62) return false;
        bits{val} /= 1;
        if (val != 62) return false;
        bits{val} /= 32;
        if (val != 1) return false;
        bits{val} /= 10;
        if (val != 0) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_div);
}

TEST_F(operators, remainder_assignment) {
    bits{ivalue} %= 10;
    EXPECT_EQ(ivalue, 1);
    bits{ivalue} %= 1;
    EXPECT_EQ(ivalue, 0);
    bits{ivalue} %= 20u;
    EXPECT_EQ(ivalue, 0);

    constexpr bool compile_time_rem = [] {
        int val = 12;
        bits{val} %= 20;
        if (val != 12) return false;
        bits{val} %= 10;
        if (val != 2) return false;
        bits{val} %= 1;
        if (val != 0) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_rem);
}

TEST_F(operators, bitwise_AND_assignment) {
    bits{ivalue} &= 0b0001;
    EXPECT_EQ(ivalue, 1);
    bits{ivalue} &= 0b0010;
    EXPECT_EQ(ivalue, 0);
    bits{ivalue} = 0b1001;
    bits{ivalue} &= 0b1000u;
    EXPECT_EQ(ivalue, 8);
    bits{ivalue} &= 0u;
    EXPECT_EQ(ivalue, 0);
    bits{ivalue} &= 0;
    EXPECT_EQ(ivalue, 0);

    constexpr bool compile_time_bit_and = [] {
        int val = 0b1111;
        bits{val} &= 0b0111;
        if (val != 0b0111) return false;
        bits{val} &= 0b0010u;
        if (val != 0b0010) return false;
        bits{val} &= 0b1111u;
        if (val != 0b0010) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_bit_and);

    bits{fvalue} &= 0xF0000000;
    EXPECT_EQ(bits{fvalue}, 0x30000000);
}

TEST_F(operators, bitwise_OR_assignment) {
    bits{ivalue} |= 0b0001;
    EXPECT_EQ(ivalue, 1);
    bits{ivalue} |= 0b1110u;
    EXPECT_EQ(ivalue, 15);
    bits{ivalue} = 0;
    bits{ivalue} |= 0b0100;
    EXPECT_EQ(ivalue, 4);
    bits{ivalue} |= 0b0001;
    EXPECT_EQ(ivalue, 5);

    constexpr bool compile_time_bit_or = [] {
        int val = 0b1100;
        bits{val} |= 0b1100;
        if (val != 0b1100) return false;
        bits{val} |= 0b1111;
        if (val != 0b1111) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_bit_or);
}

TEST_F(operators, bitwise_XOR_assignment) {
    bits{ivalue} ^= 0b1111;
    EXPECT_EQ(ivalue, 14);
    bits{ivalue} ^= 0b1010u;
    EXPECT_EQ(ivalue, 4);

    constexpr bool compile_time_bit_xor = [] {
        int val = 0b1001;
        bits{val} ^= 0b1111;
        if (val != 0b0110) return false;
        bits{val} ^= 0b0110;
        if (val != 0b0000) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_bit_xor);
}

TEST_F(operators, bitwise_left_shift_assignment) {
    bits{ivalue} <<= 1;
    EXPECT_EQ(ivalue, 2);
    bits{ivalue} <<= 2u;
    EXPECT_EQ(ivalue, 8);
    bits{ivalue} <<= 0;
    EXPECT_EQ(ivalue, 8);

    constexpr bool compile_time_lshift = [] {
        int val = 0b0100;
        bits{val} <<= 1;
        if (val != 0b1000) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_lshift);

    bits{fvalue} <<= 1;
    EXPECT_EQ(fvalue, 1.7014118346e+38f);
    bits{fvalue} <<= 1;
    EXPECT_EQ(bits{fvalue}, 0xFE000000);
}

TEST_F(operators, bitwise_right_shift_assignment) {
    bits{ivalue} >>= 1;
    EXPECT_EQ(ivalue, 0);
    bits{ivalue} = 120;
    bits{ivalue} >>= 5u;
    EXPECT_EQ(ivalue, 3);
    bits{ivalue} >>= 0u;
    EXPECT_EQ(ivalue, 3);

    constexpr bool compile_time_rshift = [] {
        int val = 0b1000;
        bits{val} >>= 1;
        if (val != 0b0100) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_rshift);

    bits{fvalue} >>= 7;
    EXPECT_EQ(bits{fvalue}, 0x007F0000);
}

TEST_F(operators, subscript) {
    EXPECT_TRUE(bits{ivalue}[0]);
    EXPECT_FALSE(bits{ivalue}[1]);

    EXPECT_FALSE(bits{const_ivalue}[0]);
    EXPECT_TRUE(bits{const_ivalue}[1]);

    EXPECT_TRUE(bits{ivalue}[0].get());

    bits{ivalue}[0] = bool(0);
    EXPECT_EQ(ivalue, 0b0000);
    bits{ivalue}[1] = bool(1);
    EXPECT_EQ(ivalue, 0b0010);
    bits{ivalue}[0] = bool(1);
    EXPECT_EQ(ivalue, 0b0011);
    bits{ivalue}[7] = bool(1);
    EXPECT_EQ(ivalue, 0b1000'0011u);

    constexpr bool compile_time_subscript = [] {
        int val = 0b1000'0000u;
        bits{val}[0] = bool(1);
        if (val != 0b1000'0001u) return false;
        if (bits{val}[0] != bool(1)) return false;

        return true;
    }();
    EXPECT_TRUE(compile_time_subscript);

    std::array<int, 4> arr{0b0111, 0b0010, 0b1000, 0b0000};
    EXPECT_TRUE(bits{arr}[0]);
    EXPECT_FALSE(bits{arr}[3]);
    EXPECT_TRUE(bits{arr}[33]);

    bits{arr}[4] = bool(1);
    EXPECT_TRUE(bits{arr}[4]);
    bits{arr}[65] = bool(1);
    EXPECT_TRUE(bits{arr}[65]);

    EXPECT_TRUE(bits{arr[1]}[1]);
    bits{arr[1]}[1] = bool(0);
    EXPECT_FALSE(bits{arr[1]}[1]);

    bits{fvalue}[31] = bool(1);
    EXPECT_EQ(fvalue, -1.f);
    bits{fvalue}[30] = bool(1);
    EXPECT_EQ(bits{fvalue}, 0xFF800000); // -inf
    bits{fvalue}[22].flip();
    EXPECT_EQ(bits{fvalue}, 0xFFC00000); // nan
}

TEST_F(operators, refw_addition_assignment) {
    bits{ivalue}.as_refw<unsigned>() += 1u;
    EXPECT_EQ(ivalue, 2);
}
TEST_F(operators, refw_subtraction_assignment) {
    auto uint_ref = bits{ivalue}.as_refw<unsigned>();
    uint_ref -= 2u;
    EXPECT_EQ(uint_ref, unsigned(-1));
    EXPECT_EQ(ivalue, -1);
}
TEST_F(operators, refw_bitwise_OR_assignment) {
    bits{ivalue}.as_refw<unsigned>() |= ~0u;
    EXPECT_EQ(ivalue, -1);
}
TEST_F(operators, refw_bitwise_left_shift_assignment) {
    bits{ivalue}.as_refw<unsigned>() <<= 31u;
    EXPECT_EQ(ivalue, -2147483648);
}

} // namespace
