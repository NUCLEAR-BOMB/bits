#include <gtest/gtest.h>
#include <bits.hpp>
#include <cstdint>

#include <test_types.hpp>

#define TypeParam T

namespace {

template<class T>
struct operators : ::testing::Test {
	T value{1};
	const T const_value{2};
	static constexpr T constexpr_value{3};
};

TYPED_TEST_SUITE(operators, test_types,);

TYPED_TEST(operators, assigment) {
	bits{this->value} = T(2);
	EXPECT_EQ(this->value, 2);
}

TYPED_TEST(operators, narrow_assigment) {
	bits{this->value}.narrow() = 2;
	EXPECT_EQ(this->value, 2);
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
	constexpr bool compile_time_equal_to_bits = bits{this->constexpr_value} == bits{this->constexpr_value};
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
	constexpr bool compile_time_not_equal_to_bits = bits{this->constexpr_value} != bits{this->constexpr_value};
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
	constexpr bool compile_time_gt_bits = bits{this->constexpr_value} > bits{this->constexpr_value};
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
	constexpr bool compile_time_gteq_bits = bits{this->constexpr_value} >= bits{this->constexpr_value};
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
	constexpr bool compile_time_lt_bits = bits{this->constexpr_value} < bits{this->constexpr_value};
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
	constexpr bool compile_time_lteq_bits = bits{this->constexpr_value} <= bits{this->constexpr_value};
	EXPECT_TRUE(compile_time_lteq_bits);

	EXPECT_FALSE(bits{this->const_value} <= bits{this->value});
	EXPECT_TRUE(bits{this->value} <= bits{this->const_value});
	EXPECT_FALSE(bits{this->constexpr_value} <= bits{this->value});
}

TYPED_TEST(operators, addition_assignment) {
	bits{this->value} += 1;
	EXPECT_EQ(this->value, 2);
	bits{this->value} += 100u;
	EXPECT_EQ(this->value, 102);
	bits{this->value} += (-2);
	EXPECT_EQ(this->value, 100);
	bits{this->value} += (-100);
	EXPECT_EQ(this->value, 0);

	constexpr bool compile_time_add = [] {
		T val = 1;
		bits{val} += 2;
		if (val != 3) return false;
		bits{val} += (-2);
		if (val != 1) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_add);
}

TYPED_TEST(operators, subtraction_assignment) {
	bits{this->value} -= 1;
	EXPECT_EQ(this->value, T(0));
	bits{this->value} -= 100u;
	EXPECT_EQ(this->value, T(-100));
	bits{this->value} -= (-10);
	EXPECT_EQ(this->value, T(-90));
	bits{this->value} -= (-100);
	EXPECT_EQ(this->value, 10);

	constexpr bool compile_time_sub = [] {
		T val = 6;
		bits{val} -= 2;
		if (val != 4) return false;
		bits{val} -= 101;
		if (val != T(-97)) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_sub);
}

TYPED_TEST(operators, multiplication_assignment) {
	bits{this->value} *= 2;
	EXPECT_EQ(this->value, 2);
	bits{this->value} *= 5u;
	EXPECT_EQ(this->value, 10);
	bits{this->value} *= -1;
	EXPECT_EQ(this->value, T(-10));

	constexpr bool compile_time_mul = [] {
		T val = 10;
		bits{val} *= 1;
		if (val != 10) return false;
		bits{val} *= 2;
		if (val != 20) return false;
		bits{val} *= -1;
		if (val != T(-20)) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_mul);
}

TYPED_TEST(operators, division_assignment) {
	bits{this->value} /= 2;
	EXPECT_EQ(this->value, 0);
	bits{this->value} = T(25);
	bits{this->value} /= -5;
	EXPECT_EQ(this->value, T(-5));
	bits{this->value} /= 5;
	EXPECT_EQ(this->value, T(-1));

	constexpr bool compile_time_div = [] {
		T val = 124;
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

TYPED_TEST(operators, remainder_assignment) {
	bits{this->value} %= 10;
	EXPECT_EQ(this->value, 1);
	bits{this->value} %= -2;
	EXPECT_EQ(this->value, 1);
	bits{this->value} %= 1;
	EXPECT_EQ(this->value, 0);
	bits{this->value} %= 20u;
	EXPECT_EQ(this->value, 0);

	constexpr bool compile_time_rem = [] {
		T val = 12;
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

TYPED_TEST(operators, bitwise_AND_assignment) {
	bits{this->value} &= 0b0001;
	EXPECT_EQ(this->value, 1);
	bits{this->value} &= 0b0010;
	EXPECT_EQ(this->value, 0);
	bits{this->value} = T(0b1001);
	bits{this->value} &= 0b1000u;
	EXPECT_EQ(this->value, 8);
	bits{this->value} &= 0u;
	EXPECT_EQ(this->value, 0);
	bits{this->value} &= 0;
	EXPECT_EQ(this->value, 0);

	constexpr bool compile_time_bit_and = [] {
		T val = 0b1111;
		bits{val} &= 0b0111;
		if (val != 0b0111) return false;
		bits{val} &= 0b0010u;
		if (val != 0b0010) return false;
		bits{val} &= 0b1111u;
		if (val != 0b0010) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_bit_and);
}

TYPED_TEST(operators, bitwise_OR_assignment) {
	bits{this->value} |= 0b0001;
	EXPECT_EQ(this->value, 1);
	bits{this->value} |= 0b1110u;
	EXPECT_EQ(this->value, 15);
	bits{this->value} = T(0);
	bits{this->value} |= 0b0100;
	EXPECT_EQ(this->value, 4);
	bits{this->value} |= 0b0001;
	EXPECT_EQ(this->value, 5);

	constexpr bool compile_time_bit_or = [] {
		T val = 0b1100;
		bits{val} |= 0b1100;
		if (val != 0b1100) return false;
		bits{val} |= 0b1111;
		if (val != 0b1111) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_bit_or);
}

TYPED_TEST(operators, bitwise_XOR_assignment) {
	bits{this->value} ^= 0b1111;
	EXPECT_EQ(this->value, 14);
	bits{this->value} ^= 0b1010u;
	EXPECT_EQ(this->value, 4);

	constexpr bool compile_time_bit_xor = [] {
		T val = 0b1001;
		bits{val} ^= 0b1111;
		if (val != 0b0110) return false;
		bits{val} ^= 0b0110;
		if (val != 0b0000) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_bit_xor);
}

TYPED_TEST(operators, bitwise_left_shift_assignment) {
	bits{this->value} <<= 1;
	EXPECT_EQ(this->value, 2);
	bits{this->value} <<= 2u;
	EXPECT_EQ(this->value, 8);
	bits{this->value} <<= 0;
	EXPECT_EQ(this->value, 8);

	constexpr bool compile_time_lshift = [] {
		T val = 0b0100;
		bits{val} <<= 1;
		if (val != 0b1000) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_lshift);
}

TYPED_TEST(operators, bitwise_right_shift_assignment) {
	bits{this->value} >>= 1;
	EXPECT_EQ(this->value, 0);
	bits{this->value} = T(120);
	bits{this->value} >>= 5u;
	EXPECT_EQ(this->value, 3);
	bits{this->value} >>= 0u;
	EXPECT_EQ(this->value, 3);

	constexpr bool compile_time_rshift = [] {
		T val = 0b1000;
		bits{val} >>= 1;
		if (val != 0b0100) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_rshift);
}

}
