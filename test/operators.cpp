#include <gtest/gtest.h>
#include <bits.hpp>
#include <cstdint>

#include <test_types.hpp>

namespace {

template<class T>
struct operators : ::testing::Test {
	T value{1};
	const T const_value{2};
	static constexpr T constexpr_value{3};
};

TYPED_TEST_SUITE(operators, test_types,);

TYPED_TEST(operators, assigment) {
	using T = TypeParam;
	bits{this->value} = T(2);
	EXPECT_EQ(this->value, 2);
}

TYPED_TEST(operators, narrow_assigment) {
	bits{this->value}.narrow() = 2;
	EXPECT_EQ(this->value, 2);
}

TYPED_TEST(operators, equal) {
	using T = TypeParam;
	EXPECT_TRUE(bits{this->value} == T(1));
	EXPECT_TRUE(bits{this->const_value} == T(2));
	static_assert(bits{this->constexpr_value} == T(3));

	EXPECT_TRUE(T(1) == bits{this->value});
	EXPECT_TRUE(T(2) == bits{this->const_value});
	static_assert(T(3) == bits{this->constexpr_value});

	EXPECT_TRUE(bits{this->value} == bits{this->value});
	EXPECT_TRUE(bits{this->const_value} == bits{this->const_value});
	static_assert(bits{this->constexpr_value} == bits{this->constexpr_value});

	EXPECT_FALSE(bits{this->value} == bits{this->const_value});
	EXPECT_FALSE(bits{this->const_value} == bits{this->value});
	EXPECT_FALSE(bits{this->value} == bits{this->constexpr_value});

	EXPECT_TRUE(bits{1.f} == bits{1.f});

	int array_1[8]{1, 0};
	int array_2[8]{0, 1};
	EXPECT_TRUE(bits{array_1} < bits{array_2});
}

TYPED_TEST(operators, not_equal) {
	using T = TypeParam;
	EXPECT_FALSE(bits{this->value} != T(1));
	EXPECT_FALSE(bits{this->const_value} != T(2));
	static_assert(!(bits{this->constexpr_value} != T(3)));

	EXPECT_FALSE(bits{this->value} != bits{this->value});
	EXPECT_FALSE(bits{this->const_value} != bits{this->const_value});
	static_assert(!(bits{this->constexpr_value} != bits{this->constexpr_value}));

	EXPECT_TRUE(bits{this->value} != bits{this->const_value});
	EXPECT_TRUE(bits{this->const_value} != bits{this->value});
	EXPECT_TRUE(bits{this->value} != bits{this->constexpr_value});
}

TYPED_TEST(operators, greater) {
	using T = TypeParam;
	EXPECT_TRUE(bits{this->value} > T(0));
	EXPECT_TRUE(bits{this->const_value} > T(1));
	static_assert(bits{this->constexpr_value} > T(2));

	EXPECT_FALSE(bits{this->value} > bits{this->value});
	EXPECT_FALSE(bits{this->const_value} > bits{this->const_value});
	static_assert(!(bits{this->constexpr_value} > bits{this->constexpr_value}));

	EXPECT_TRUE(bits{this->const_value} > bits{this->value});
	EXPECT_FALSE(bits{this->value} > bits{this->const_value});
	EXPECT_TRUE(bits{this->constexpr_value} > bits{this->value});
}

TYPED_TEST(operators, greater_equal) {
	using T = TypeParam;
	EXPECT_TRUE(bits{this->value} >= T(1));
	EXPECT_TRUE(bits{this->const_value} >= T(2));
	static_assert(bits{this->constexpr_value} >= T(2));

	EXPECT_TRUE(bits{this->value} >= bits{this->value});
	EXPECT_TRUE(bits{this->const_value} >= bits{this->const_value});
	static_assert(bits{this->constexpr_value} >= bits{this->constexpr_value});

	EXPECT_TRUE(bits{this->const_value} >= bits{this->value});
	EXPECT_FALSE(bits{this->value} >= bits{this->const_value});
	EXPECT_TRUE(bits{this->constexpr_value} >= bits{this->value});
}

TYPED_TEST(operators, less) {
	using T = TypeParam;
	EXPECT_TRUE(bits{this->value} < T(2));
	EXPECT_TRUE(bits{this->const_value} < T(3));
	static_assert(bits{this->constexpr_value} < T(4));

	EXPECT_FALSE(bits{this->value} < bits{this->value});
	EXPECT_FALSE(bits{this->const_value} < bits{this->const_value});
	static_assert(!(bits{this->constexpr_value} < bits{this->constexpr_value}));

	EXPECT_FALSE(bits{this->const_value} < bits{this->value});
	EXPECT_TRUE(bits{this->value} < bits{this->const_value});
	EXPECT_FALSE(bits{this->constexpr_value} < bits{this->value});
}

TYPED_TEST(operators, less_equal) {
	using T = TypeParam;
	EXPECT_TRUE(bits{this->value} <= T(1));
	EXPECT_TRUE(bits{this->const_value} <= T(2));
	static_assert(bits{this->constexpr_value} <= T(4));

	EXPECT_TRUE(bits{this->value} <= bits{this->value});
	EXPECT_TRUE(bits{this->const_value} <= bits{this->const_value});
	static_assert(bits{this->constexpr_value} <= bits{this->constexpr_value});

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
}

TYPED_TEST(operators, subtraction_assignment) {
	bits{this->value} -= 1;
	EXPECT_EQ(this->value, 0);
	bits{this->value} -= 100u;
	EXPECT_EQ(this->value, -100);
	bits{this->value} -= (-10);
	EXPECT_EQ(this->value, -90);
	bits{this->value} -= (-100);
	EXPECT_EQ(this->value, 10);
}

TYPED_TEST(operators, multiplication_assignment) {
	bits{this->value} *= 2;
	EXPECT_EQ(this->value, 2);
	bits{this->value} *= 5u;
	EXPECT_EQ(this->value, 10);
	bits{this->value} *= -1;
	EXPECT_EQ(this->value, -10);
}

TYPED_TEST(operators, division_assignment) {
	bits{this->value} /= 2;
	EXPECT_EQ(this->value, 0);
	bits{this->value} = TypeParam(25);
	bits{this->value} /= -5;
	EXPECT_EQ(this->value, -5);
	bits{this->value} /= 5;
	EXPECT_EQ(this->value, -1);
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
}

TYPED_TEST(operators, bitwise_AND_assignment) {
	bits{this->value} &= 0b0001;
	EXPECT_EQ(this->value, 1);
	bits{this->value} &= 0b0010;
	EXPECT_EQ(this->value, 0);
	bits{this->value} = TypeParam(0b1001);
	bits{this->value} &= 0b1000u;
	EXPECT_EQ(this->value, 8);
	bits{this->value} &= 0u;
	EXPECT_EQ(this->value, 0);
	bits{this->value} &= 0;
	EXPECT_EQ(this->value, 0);
}

TYPED_TEST(operators, bitwise_OR_assignment) {
	bits{this->value} |= 0b0001;
	EXPECT_EQ(this->value, 1);
	bits{this->value} |= 0b1110u;
	EXPECT_EQ(this->value, 15);
	bits{this->value} = TypeParam(0);
	bits{this->value} |= 0b0100;
	EXPECT_EQ(this->value, 4);
	bits{this->value} |= 0b0001;
	EXPECT_EQ(this->value, 5);
}

TYPED_TEST(operators, bitwise_XOR_assignment) {
	bits{this->value} ^= 0b1111;
	EXPECT_EQ(this->value, 14);
	bits{this->value} ^= 0b1010u;
	EXPECT_EQ(this->value, 4);
}

TYPED_TEST(operators, bitwise_left_shift_assignment) {
	bits{this->value} <<= 1;
	EXPECT_EQ(this->value, 2);
	bits{this->value} <<= 2u;
	EXPECT_EQ(this->value, 8);
	bits{this->value} <<= 0;
	EXPECT_EQ(this->value, 8);
}

TYPED_TEST(operators, bitwise_right_shift_assignment) {
	bits{this->value} >>= 1;
	EXPECT_EQ(this->value, 0);
	bits{this->value} = TypeParam(120);
	bits{this->value} >>= 5u;
	EXPECT_EQ(this->value, 3);
	bits{this->value} >>= 0u;
	EXPECT_EQ(this->value, 3);
}

}
