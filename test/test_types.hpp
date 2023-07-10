#pragma once

#include <cstdint>
#include <gtest/gtest.h>

using test_types = ::testing::Types<
	std::int8_t, std::int16_t, std::int32_t, std::int64_t
	//std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
>;
