#include <gtest/gtest.h>
#include <bits.hpp>

namespace {

struct operators : ::testing::Test {
	int value = 1;
	const int const_value = 2;
	static constexpr int constexpr_value = 3;
};

TEST_F(operators, assigment) {
	bits{value} = 2;
	EXPECT_EQ(value, 2);
}

TEST_F(operators, equal) {
	EXPECT_TRUE(bits{value} == 1);
	EXPECT_TRUE(bits{const_value} == 2);
	static_assert(bits{constexpr_value} == 3);

	EXPECT_TRUE(1 == bits{value});
	EXPECT_TRUE(2 == bits{const_value});
	static_assert(3 == bits{constexpr_value});
}

}
