#pragma once

#include <type_traits>
#include <cstring>

template<class T>
class bits {
public:
	using value_type = std::remove_const_t<T>;

	explicit constexpr bits(T& val) noexcept : m_value(val) {}

	bits(const bits&) = delete;
	bits& operator=(const bits&) = delete;
	bits(T&&) = delete;

	constexpr T& value() { return m_value; }
	constexpr const T& value() const { return m_value; }

private:
	template<class Dest, class Src>
	static void bit_copy(Dest* const dest, const Src* const src, const std::size_t count) {
		std::memcpy(dest, src, count);
	}

public:

private:
	T& m_value;
};

template<class T>
bits(T&) -> bits<T>;
