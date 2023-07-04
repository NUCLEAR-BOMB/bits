#include <gtest/gtest.h>

#include <bits.hpp>

namespace {

TEST(basic, constructor) {
	int value = 1;
	(void)bits{value};

	const int cvalue = 2;
	(void)bits{cvalue};

	constexpr int cevalue = 3;
	static_assert(((void)bits{cevalue}, true));
}

}
