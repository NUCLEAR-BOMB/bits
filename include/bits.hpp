#pragma once

#include <type_traits>
#include <cstring>

template<class Value>
class bits {
public:
	using value_type = std::remove_const_t<Value>;

	explicit constexpr bits(Value& val) noexcept : m_value(val) {}

	bits(Value&&) = delete;

	constexpr Value& value() { return m_value; }
	constexpr const Value& value() const { return m_value; }

private:
	template<class To, class From>
	constexpr static void bit_cast_to(To& to, const From& from) {
		static_assert(sizeof(To) == sizeof(From));
		if constexpr (std::is_trivially_assignable_v<To, From>) {
			to = from;
		} else {
			std::memcpy(&to, &from, sizeof(To));
		}
	}
	template<class To, class From>
	constexpr static void bit_cast_to(const To&, const From&) = delete;

public:

	template<class T>
	constexpr bits& operator=(const T& other) {
		bit_cast_to(m_value, other);
		return *this;
	}

private:
	Value& m_value;
};

template<class Value>
bits(Value&) -> bits<Value>;
