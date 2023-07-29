#include <bits.hpp>
#include <cmath>
#include <cstdint>
#include <gtest/gtest.h>
#include <test_types.hpp>

#define TypeParam T

namespace {

template<class T>
struct operators : ::testing::Test {
    T value = T(1);
    const T const_value = T(2);
    static constexpr T constexpr_value = T(3);
};

using float_operators = operators<float>;
using int_operators = operators<int>;

TYPED_TEST_SUITE(operators, test_types, );

TYPED_TEST(operators, assigment) {
    bits{this->value} = T(2);
    EXPECT_EQ(this->value, T(2));
}

TYPED_TEST(operators, narrow_assigment) {
    bits{this->value}.narrow() = 2;
    EXPECT_EQ(this->value, T(2));
}

TYPED_TEST(operators, equal) {
    EXPECT_TRUE(bits{this->value} == T(1));
    EXPECT_TRUE(bits{this->const_value} == T(2));
    constexpr bool compile_time_equal_to_value = bits{this->constexpr_value} == T(3);
    EXPECT_TRUE(compile_time_equal_to_value);

    EXPECT_TRUE(T(1) == bits{this->value});
    EXPECT_TRUE(T(2) == bits{this->const_value});
    constexpr bool compile_time_yoda_equal_to_value = T(3) == bits{this->constexpr_value};
    EXPECT_TRUE(compile_time_yoda_equal_to_value);

    EXPECT_TRUE(bits{this->value} == bits{this->value});
    EXPECT_TRUE(bits{this->const_value} == bits{this->const_value});
    constexpr bool compile_time_equal_to_bits
        = bits{this->constexpr_value} == bits{this->constexpr_value};
    EXPECT_TRUE(compile_time_equal_to_bits);

    EXPECT_FALSE(bits{this->value} == bits{this->const_value});
    EXPECT_FALSE(bits{this->const_value} == bits{this->value});
    EXPECT_FALSE(bits{this->value} == bits{this->constexpr_value});

    EXPECT_TRUE(bits{1.f} == bits{1.f});

    int array_1[8]{1, 0};
    int array_2[8]{0, 1};
    EXPECT_TRUE(bits{array_1} < bits{array_2});
}

TYPED_TEST(operators, not_equal) {
    EXPECT_FALSE(bits{this->value} != T(1));
    EXPECT_FALSE(bits{this->const_value} != T(2));
    constexpr bool compile_time_not_equal_to_value = bits{this->constexpr_value} != T(3);
    EXPECT_FALSE(compile_time_not_equal_to_value);

    EXPECT_FALSE(bits{this->value} != bits{this->value});
    EXPECT_FALSE(bits{this->const_value} != bits{this->const_value});
    constexpr bool compile_time_not_equal_to_bits
        = bits{this->constexpr_value} != bits{this->constexpr_value};
    EXPECT_FALSE(compile_time_not_equal_to_bits);

    EXPECT_TRUE(bits{this->value} != bits{this->const_value});
    EXPECT_TRUE(bits{this->const_value} != bits{this->value});
    EXPECT_TRUE(bits{this->value} != bits{this->constexpr_value});
}

TYPED_TEST(operators, greater) {
    EXPECT_TRUE(bits{this->value} > T(0));
    EXPECT_TRUE(bits{this->const_value} > T(1));
    constexpr bool compile_time_gt_value = bits{this->constexpr_value} > T(2);
    EXPECT_TRUE(compile_time_gt_value);

    EXPECT_FALSE(bits{this->value} > bits{this->value});
    EXPECT_FALSE(bits{this->const_value} > bits{this->const_value});
    constexpr bool compile_time_gt_bits
        = bits{this->constexpr_value} > bits{this->constexpr_value};
    EXPECT_FALSE(compile_time_gt_bits);

    EXPECT_TRUE(bits{this->const_value} > bits{this->value});
    EXPECT_FALSE(bits{this->value} > bits{this->const_value});
    EXPECT_TRUE(bits{this->constexpr_value} > bits{this->value});
}

TYPED_TEST(operators, greater_equal) {
    EXPECT_TRUE(bits{this->value} >= T(1));
    EXPECT_TRUE(bits{this->const_value} >= T(2));
    constexpr bool compile_time_gteq_value = bits{this->constexpr_value} >= T(2);
    EXPECT_TRUE(compile_time_gteq_value);

    EXPECT_TRUE(bits{this->value} >= bits{this->value});
    EXPECT_TRUE(bits{this->const_value} >= bits{this->const_value});
    constexpr bool compile_time_gteq_bits
        = bits{this->constexpr_value} >= bits{this->constexpr_value};
    EXPECT_TRUE(compile_time_gteq_bits);

    EXPECT_TRUE(bits{this->const_value} >= bits{this->value});
    EXPECT_FALSE(bits{this->value} >= bits{this->const_value});
    EXPECT_TRUE(bits{this->constexpr_value} >= bits{this->value});
}

TYPED_TEST(operators, less) {
    EXPECT_TRUE(bits{this->value} < T(2));
    EXPECT_TRUE(bits{this->const_value} < T(3));
    constexpr bool compile_time_lt_value = bits{this->constexpr_value} < T(4);
    EXPECT_TRUE(compile_time_lt_value);

    EXPECT_FALSE(bits{this->value} < bits{this->value});
    EXPECT_FALSE(bits{this->const_value} < bits{this->const_value});
    constexpr bool compile_time_lt_bits
        = bits{this->constexpr_value} < bits{this->constexpr_value};
    EXPECT_FALSE(compile_time_lt_bits);

    EXPECT_FALSE(bits{this->const_value} < bits{this->value});
    EXPECT_TRUE(bits{this->value} < bits{this->const_value});
    EXPECT_FALSE(bits{this->constexpr_value} < bits{this->value});
}

TYPED_TEST(operators, less_equal) {
    EXPECT_TRUE(bits{this->value} <= T(1));
    EXPECT_TRUE(bits{this->const_value} <= T(2));
    constexpr bool compile_time_lteq_value = bits{this->constexpr_value} <= T(4);
    EXPECT_TRUE(compile_time_lteq_value);

    EXPECT_TRUE(bits{this->value} <= bits{this->value});
    EXPECT_TRUE(bits{this->const_value} <= bits{this->const_value});
    constexpr bool compile_time_lteq_bits
        = bits{this->constexpr_value} <= bits{this->constexpr_value};
    EXPECT_TRUE(compile_time_lteq_bits);

    EXPECT_FALSE(bits{this->const_value} <= bits{this->value});
    EXPECT_TRUE(bits{this->value} <= bits{this->const_value});
    EXPECT_FALSE(bits{this->constexpr_value} <= bits{this->value});
}

TYPED_TEST(operators, addition_assignment) {
    bits{this->value} += 1;
    EXPECT_EQ(this->value, T(2));
    bits{this->value} += 100u;
    EXPECT_EQ(this->value, T(102));
    bits{this->value} += uint_as<T>(-2);
    EXPECT_EQ(this->value, T(100));
    bits{this->value} += uint_as<T>(-100);
    EXPECT_EQ(this->value, T(0));

    constexpr bool compile_time_add = [] {
        T val = T(1);
        bits{val} += 2;
        if (val != T(3)) return false;
        bits{val} += uint_as<T>(-2);
        if (val != T(1)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_add);
}

TYPED_TEST(operators, subtraction_assignment) {
    bits{this->value} -= 1;
    EXPECT_EQ(this->value, T(0));
    bits{this->value} -= 100u;
    EXPECT_EQ(this->value, T(-100));
    bits{this->value} -= uint_as<T>(-10);
    EXPECT_EQ(this->value, T(-90));
    bits{this->value} -= uint_as<T>(-100);
    EXPECT_EQ(this->value, T(10));

    constexpr bool compile_time_sub = [] {
        T val = T(6);
        bits{val} -= 2;
        if (val != T(4)) return false;
        bits{val} -= 101;
        if (val != T(-97)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_sub);
}

TYPED_TEST(operators, multiplication_assignment) {
    bits{this->value} *= 2;
    EXPECT_EQ(this->value, T(2));
    bits{this->value} *= 5u;
    EXPECT_EQ(this->value, T(10));
    bits{this->value} *= 0;
    EXPECT_EQ(this->value, T(0));

    constexpr bool compile_time_mul = [] {
        T val = T(10);
        bits{val} *= 1;
        if (val != T(10)) return false;
        bits{val} *= 2;
        if (val != T(20)) return false;
        bits{val} *= 0;
        if (val != T(0)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_mul);
}

TYPED_TEST(operators, division_assignment) {
    bits{this->value} /= 2;
    EXPECT_EQ(this->value, T(0));
    bits{this->value} = T(25);
    bits{this->value} /= 5;
    EXPECT_EQ(this->value, T(5));

    constexpr bool compile_time_div = [] {
        T val = T(124);
        bits{val} /= 2;
        if (val != T(62)) return false;
        bits{val} /= 1;
        if (val != T(62)) return false;
        bits{val} /= 32;
        if (val != T(1)) return false;
        bits{val} /= 10;
        if (val != T(0)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_div);
}

TYPED_TEST(operators, remainder_assignment) {
    bits{this->value} %= 10;
    EXPECT_EQ(this->value, T(1));
    bits{this->value} %= 1;
    EXPECT_EQ(this->value, T(0));
    bits{this->value} %= 20u;
    EXPECT_EQ(this->value, T(0));

    constexpr bool compile_time_rem = [] {
        T val = T(12);
        bits{val} %= 20;
        if (val != T(12)) return false;
        bits{val} %= 10;
        if (val != T(2)) return false;
        bits{val} %= 1;
        if (val != T(0)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_rem);
}

TYPED_TEST(operators, bitwise_AND_assignment) {
    bits{this->value} &= 0b0001;
    EXPECT_EQ(this->value, T(1));
    bits{this->value} &= 0b0010;
    EXPECT_EQ(this->value, T(0));
    bits{this->value} = T(0b1001);
    bits{this->value} &= 0b1000u;
    EXPECT_EQ(this->value, T(8));
    bits{this->value} &= 0u;
    EXPECT_EQ(this->value, T(0));
    bits{this->value} &= 0;
    EXPECT_EQ(this->value, T(0));

    constexpr bool compile_time_bit_and = [] {
        T val = T(0b1111);
        bits{val} &= 0b0111;
        if (val != T(0b0111)) return false;
        bits{val} &= 0b0010u;
        if (val != T(0b0010)) return false;
        bits{val} &= 0b1111u;
        if (val != T(0b0010)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_bit_and);
}

TYPED_TEST(operators, bitwise_OR_assignment) {
    bits{this->value} |= 0b0001;
    EXPECT_EQ(this->value, T(1));
    bits{this->value} |= 0b1110u;
    EXPECT_EQ(this->value, T(15));
    bits{this->value} = T(0);
    bits{this->value} |= 0b0100;
    EXPECT_EQ(this->value, T(4));
    bits{this->value} |= 0b0001;
    EXPECT_EQ(this->value, T(5));

    constexpr bool compile_time_bit_or = [] {
        T val = T(0b1100);
        bits{val} |= 0b1100;
        if (val != T(0b1100)) return false;
        bits{val} |= 0b1111;
        if (val != T(0b1111)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_bit_or);
}

TYPED_TEST(operators, bitwise_XOR_assignment) {
    bits{this->value} ^= 0b1111;
    EXPECT_EQ(this->value, T(14));
    bits{this->value} ^= 0b1010u;
    EXPECT_EQ(this->value, T(4));

    constexpr bool compile_time_bit_xor = [] {
        T val = T(0b1001);
        bits{val} ^= 0b1111;
        if (val != T(0b0110)) return false;
        bits{val} ^= 0b0110;
        if (val != T(0b0000)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_bit_xor);
}

TYPED_TEST(operators, bitwise_left_shift_assignment) {
    bits{this->value} <<= 1;
    EXPECT_EQ(this->value, T(2));
    bits{this->value} <<= 2u;
    EXPECT_EQ(this->value, T(8));
    bits{this->value} <<= 0;
    EXPECT_EQ(this->value, T(8));

    constexpr bool compile_time_lshift = [] {
        T val = T(0b0100);
        bits{val} <<= 1;
        if (val != T(0b1000)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_lshift);
}

TYPED_TEST(operators, bitwise_right_shift_assignment) {
    bits{this->value} >>= 1;
    EXPECT_EQ(this->value, T(0));
    bits{this->value} = T(120);
    bits{this->value} >>= 5u;
    EXPECT_EQ(this->value, T(3));
    bits{this->value} >>= 0u;
    EXPECT_EQ(this->value, T(3));

    constexpr bool compile_time_rshift = [] {
        T val = T(0b1000);
        bits{val} >>= 1;
        if (val != T(0b0100)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_rshift);
}

TYPED_TEST(operators, subscript) {
    EXPECT_TRUE(bits{this->value}[0]);
    EXPECT_FALSE(bits{this->value}[1]);

    EXPECT_FALSE(bits{this->const_value}[0]);
    EXPECT_TRUE(bits{this->const_value}[1]);

    EXPECT_TRUE(bits{this->constexpr_value}[0]);
    EXPECT_TRUE(bits{this->constexpr_value}[1]);
    EXPECT_FALSE(bits{this->constexpr_value}[2]);

    EXPECT_TRUE(bits{this->value}[0].get());

    bits{this->value}[0] = bool(0);
    EXPECT_EQ(this->value, T(0b0000));
    bits{this->value}[1] = bool(1);
    EXPECT_EQ(this->value, T(0b0010));
    bits{this->value}[0] = bool(1);
    EXPECT_EQ(this->value, T(0b0011));
    bits{this->value}[7] = bool(1);
    EXPECT_EQ(this->value, T(0b1000'0011u));

    auto array = weak_make_array<T, 4>(0b0111, 0b0010, 0b1000);
    EXPECT_TRUE(bits{array}[0]);
    EXPECT_FALSE(bits{array}[3]);

    EXPECT_TRUE(bits{array}[CHAR_BIT * sizeof(T) + 1]);

    bits{array}[4] = bool(1);
    EXPECT_TRUE(bits{array}[4]);
    bits{array}[CHAR_BIT * sizeof(T) * 2 + 1] = 1;
    EXPECT_TRUE(bits{array}[CHAR_BIT * sizeof(T) * 2 + 1]);

    EXPECT_TRUE(bits{array[1]}[1]);
    bits{array[1]}[1] = bool(0);
    EXPECT_FALSE(bits{array[1]}[1]);

    constexpr bool compile_time_subscript = [] {
        T val = T(0b1000'0000u);
        bits{val}[0] = 1;
        if (val != T(0b1000'0001u)) return false;
        if (bits{val}[0] != 1) return false;

        return true;
    }();
    EXPECT_TRUE(compile_time_subscript);
}

// https://www.h-schmidt.net/FloatConverter/IEEE754.html

TEST_F(float_operators, addition_assignment) {
    bits{value} += 1 << 23;
    EXPECT_EQ(value, 2.f);
    bits{value} += 1 << 22;
    EXPECT_EQ(value, 3.f);
}

TEST_F(float_operators, subtraction_assignment) {
    bits{value} -= 1 << 22;
    EXPECT_EQ(value, 0.75f);
    bits{value} -= 1u << 30;
    EXPECT_EQ(bits{value}, 0xFF400000);
}

TEST_F(float_operators, bitwise_AND_assignment) {
    bits{value} &= 0xF0000000;
    EXPECT_EQ(bits{value}, 0x30000000);
}

TEST_F(float_operators, bitwise_left_shift_assignment) {
    bits{value} <<= 1;
    EXPECT_EQ(value, 1.7014118346e+38f);
    bits{value} <<= 1;
    EXPECT_EQ(bits{value}, 0xFE000000);
}

TEST_F(float_operators, bitwise_right_shift_assignment) {
    bits{value} >>= 7;
    EXPECT_EQ(bits{value}, 0x007F0000);
}

TEST_F(float_operators, subscript) {
    bits{value}[31] = bool(1);
    EXPECT_EQ(value, -1.f);
    bits{value}[30] = bool(1);
    EXPECT_EQ(bits{value}, 0xFF800000); // -inf
    bits{value}[22].flip();
    EXPECT_EQ(bits{value}, 0xFFC00000); // nan
}

TEST_F(int_operators, refw_addition_assignment) {
    bits{value}.as_refw<unsigned>() += 1u;
    EXPECT_EQ(value, 2);
}
TEST_F(int_operators, refw_subtraction_assignment) {
    auto uint_ref = bits{value}.as_refw<unsigned>();
    uint_ref -= 2u;
    EXPECT_EQ(uint_ref, unsigned(-1));
    EXPECT_EQ(value, -1);
}
TEST_F(int_operators, refw_bitwise_OR_assignment) {
    bits{value}.as_refw<unsigned>() |= ~0u;
    EXPECT_EQ(value, -1);
}
TEST_F(int_operators, refw_bitwise_left_shift_assignment) {
    bits{value}.as_refw<unsigned>() <<= 31u;
    EXPECT_EQ(value, -2147483648);
}

} // namespace
