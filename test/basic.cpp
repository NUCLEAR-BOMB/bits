#include <gtest/gtest.h>

#include <bits.hpp>

namespace {

struct basic : ::testing::Test {
	int value = 1;
	const int const_value = 2;
	static constexpr int constexpr_value = 3;
};

TEST_F(basic, constructor) {
	(void)bits{value};
	(void)bits{const_value};
	static_assert(((void)bits{constexpr_value}, true));
}

TEST_F(basic, value_method) {
	EXPECT_EQ(bits{value}.value(), value);
	EXPECT_EQ(bits{const_value}.value(), const_value);
	static_assert(bits{constexpr_value}.value() == constexpr_value);
}

}
