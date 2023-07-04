#pragma once

#include <type_traits>

template<class T>
class bits {
public:
	using value_type = std::remove_const_t<T>;

	explicit constexpr bits(T& val) noexcept : m_value(val) {}

	bits(const bits&) = delete;
	bits& operator=(const bits&) = delete;
	bits(T&&) = delete;

private:
	T& m_value;
};

template<class T>
bits(T&) -> bits<T>;
