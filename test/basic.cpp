#include <gtest/gtest.h>

#define BITS_COMPILER_INTRINSICS 1
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

TEST_F(basic, uint_method) {
	using type = decltype(bits{value}.uint());
	static_assert(std::is_unsigned_v<type>);

	EXPECT_EQ(bits{value}.uint(), value);
	EXPECT_EQ(bits{const_value}.uint(), const_value);
	static_assert(bits{constexpr_value}.uint() == constexpr_value);
}

TEST_F(basic, constructor_in_place) {
	(void)bits{1};
	(void)bits{1.f};
}

}
