#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#if __cplusplus >= 202002L
    #include <version>
#endif

#ifdef _MSC_VER
    #include <intrin.h>
#endif

#if defined(BITS_COMPILER_INTRINSICS) && BITS_COMPILER_INTRINSICS
    #define BITS_ENABLE_INTRINSICS 1
    #ifdef __has_builtin
        #define BITS_HAS_BUILTIN(x) __has_builtin(x)
    #else
        #define BITS_HAS_BUILTIN(x) 0
    #endif
#else
    #define BITS_ENABLE_INTRINSICS 0
    #define BITS_HAS_BUILTIN(x) 0
#endif

#if BITS_ENABLE_INTRINSICS && (defined(_MSC_VER) || BITS_HAS_BUILTIN(__builtin_bit_cast))
    #define BITS_HAS_BIT_CAST_INTRINSICS 1
#else
    #define BITS_HAS_BIT_CAST_INTRINSICS 0
#endif
#if BITS_ENABLE_INTRINSICS \
    && (defined(_MSC_VER) || BITS_HAS_BUILTIN(__builtin_is_constant_evaluated))
    #define BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS 1
#else
    #define BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS 0
#endif

template<class Value = int>
class bits {
public:
    using value_type = std::remove_const_t<Value>;

    explicit constexpr bits(Value& val) noexcept : m_value(val) {}

    [[nodiscard]] constexpr Value& value() const { return m_value; }

private:
    //#if defined(UINT64_MAX) && UINT64_MAX == UINTMAX_MAX
    //	template<> struct size_as_uint_t<8> { using type = std::uint64_t; };
    //#endif

    template<class T>
    struct type_identity {
        using type = T;
    };
    template<class T>
    using type_identity_t = typename type_identity<T>::type;

#if defined(UINT64_MAX) && UINT64_MAX <= UINTMAX_MAX
    using biggest_uint_t = std::uint64_t;
#else
    using biggest_uint_t = std::uintmax_t;
#endif

    template<std::size_t N>
    static decltype(auto) size_as_uint_impl() {
        if constexpr (N <= 1) return std::uint8_t{};
        else if constexpr (N <= 2) return std::uint16_t{};
        else if constexpr (N <= 4) return std::uint32_t{};
        else if constexpr (N <= 8) return std::uint64_t{};
        else return biggest_uint_t{};
    }

    template<std::size_t N>
    using size_as_uint = decltype(size_as_uint_impl<N>());
    template<class T>
    using type_as_uint = size_as_uint<sizeof(T)>;
    template<class T>
    using type_as_int = std::make_signed_t<type_as_uint<T>>;

    using value_as_uint_type = type_as_uint<value_type>;

    template<class ArrayType, class T>
    using type_as_array = std::array<ArrayType, sizeof(T) / sizeof(ArrayType)>;

    template<class T, bool = std::is_enum_v<T>>
    struct safe_underlying_type_impl {
        using type = std::underlying_type_t<T>;
    };
    template<class T>
    struct safe_underlying_type_impl<T, false> {
        using type = T;
    };

    template<class T>
    using safe_underlying_type = typename safe_underlying_type_impl<T>::type;

    template<class T>
    static constexpr bool is_like_integral = std::is_integral_v<T> || std::is_enum_v<T>;

    template<class T>
    static constexpr bool is_convertible_as_uint = (sizeof(T) <= sizeof(biggest_uint_t));

    template<class T>
    static constexpr bool is_array_that_convertible_as_uint = false;
    template<class T, std::size_t N>
    static constexpr bool is_array_that_convertible_as_uint<std::array<T, N>>
        = is_convertible_as_uint<T>;
    template<class T, std::size_t N>
    static constexpr bool is_array_that_convertible_as_uint<T[N]> = is_convertible_as_uint<T>;

    template<class From, class To>
    static constexpr bool is_bit_convertible = is_like_integral<From> && is_like_integral<To>;

    template<class T>
    static constexpr T denominator_of_power2(const T x) {
        const auto result = T(x & (~(x - 1)));
        return result > sizeof(biggest_uint_t) ? sizeof(biggest_uint_t) : result;
    }

    template<class T>
    static constexpr bool is_integer_array = false;
    template<class T, std::size_t N>
    static constexpr bool is_integer_array<std::array<T, N>>
        = std::is_integral_v<T> || std::is_enum_v<T>;

    template<class T, std::size_t N>
    static constexpr bool is_integer_array<T[N]> = std::is_integral_v<T> || std::is_enum_v<T>;

    template<class T, std::size_t Size = denominator_of_power2(sizeof(T))>
    using as_max_size_array = std::array<size_as_uint<Size>, sizeof(T) / Size>;

    using bitsize_t = biggest_uint_t;

    template<class T>
    using array_value_type = std::decay_t<decltype(std::declval<T&>()[std::size_t(0)])>;

    template<class To, class From>
    using const_like = std::conditional_t<std::is_const_v<From>, std::add_const_t<To>,
                                          std::remove_const_t<To>>;

    template<class T>
    static constexpr decltype(auto) unwrap(const bits<T>& val) {
        return val.value();
    }
    template<class T>
    static constexpr decltype(auto) unwrap(const T& val) {
        return val;
    }

    static constexpr bool is_constant_evaluated() {
#ifdef __cpp_lib_is_constant_evaluated
        return std::is_constant_evaluated();
#elif BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS
        return __builtin_is_constant_evaluated();
#else
        return false;
#endif
    }

    template<class To, class From>
    static constexpr void bit_copy(To& dest, const From& src) {
        static_assert(sizeof(To) == sizeof(From));
        if constexpr (is_integer_array<To> && is_like_integral<From>) {
            using elem_type = array_value_type<To>;
            using int_type = safe_underlying_type<From>;

            for (std::size_t i = 0; i < sizeof(To) / sizeof(elem_type); ++i) {
                dest[i] = elem_type(int_type(src) >> (i * sizeof(elem_type) * CHAR_BIT));
            }
        } else {
            std::memcpy(std::addressof(dest), std::addressof(src), sizeof(To));
        }
    }

    template<class To, class From>
    static constexpr To bit_cast(const From& from) {
        static_assert(sizeof(To) == sizeof(From));
#ifdef __cpp_lib_bit_cast
        return std::bit_cast<To>(from);
#elif BITS_HAS_BIT_CAST_INTRINSICS
        return __builtin_bit_cast(To, from);
#else
        static_assert(std::is_copy_constructible_v<To>, "Type must be copy constructible");
        if constexpr (std::is_same_v<To, From> && std::is_trivially_copy_constructible_v<To>) {
            return from;
        } else if constexpr (is_bit_convertible<From, To>) {
            return static_cast<To>(from);
        } else if constexpr (std::is_default_constructible_v<To>) {
            To result{};
            bit_copy(result, from);
            return result;
        } else {
            return reinterpret_cast<const To&>(from);
        }
#endif
    }

    template<class To, class From>
    static constexpr void bit_cast_to(To& to, const From& from) {
        static_assert(sizeof(To) == sizeof(From));
        if constexpr (std::is_trivially_copy_assignable_v<To>) {
            to = bit_cast<To>(from);
        } else {
            bit_copy(to, from);
        }
    }
    template<class To, class From>
    static constexpr void bit_cast_to(const To&, const From&) = delete;

    template<class To, class From>
    static constexpr void expand_bit_cast_to(To& to, const From& from) {
        static_assert(sizeof(To) >= sizeof(From));

        if constexpr (sizeof(To) == sizeof(From)) {
            bit_cast_to(to, from);
        } else {
            std::memcpy(std::addressof(to), std::addressof(from), sizeof(From));
            const auto to_as_bytes = reinterpret_cast<std::byte*>(std::addressof(to));
            std::memset(to_as_bytes + sizeof(From), 0, sizeof(To) - sizeof(From));
        }
    }
    template<class To, class From>
    static constexpr void expand_bit_cast_to(const To&, const From&) = delete;

    template<class To, class From>
    static constexpr void narrow_bit_cast_to(To& to, const From& from) {
        if constexpr (is_bit_convertible<From, To>
                      && std::is_trivially_copy_assignable_v<To>) {
            to = static_cast<To>(from);
        } else if constexpr (sizeof(To) >= sizeof(From)) {
            expand_bit_cast_to(to, from);
        } else {
            std::memcpy(std::addressof(to), std::addressof(from), sizeof(To));
        }
    }
    template<class To, class From>
    static constexpr void narrow_bit_cast_to(const To&, const From&) = delete;

    template<class Fn, class Left, class Right>
    static constexpr bool bit_compare(const Left& left, const Right& right) {
        static_assert(sizeof(Left) == sizeof(Right));
        using T = Left;
        if constexpr (is_like_integral<Left> && is_like_integral<Right>) {
            return Fn{}(type_as_uint<Left>(left), type_as_uint<Right>(right));
        } else if constexpr (is_convertible_as_uint<T>) {
            using uint_type = type_as_uint<T>;
            return Fn{}(bit_cast<uint_type>(left), bit_cast<uint_type>(right));
        } else {
            using as_array_type = as_max_size_array<T>;
            return Fn{}(bit_cast<as_array_type>(left), bit_cast<as_array_type>(right));
        }
    }

    template<class T, class Fn>
    static constexpr void bit_apply(T& value, Fn fn) {
        auto buffer = bit_cast<std::array<unsigned char, sizeof(T)>>(value);
        std::move(fn)(buffer);
        bit_cast_to(value, buffer);
    }
    template<class T, class Fn>
    static constexpr decltype(auto) bit_apply(const T& value, Fn fn) {
        const auto buffer = bit_cast<std::array<unsigned char, sizeof(T)>>(value);
        return std::move(fn)(buffer);
    }

    template<bool SetTo, class T>
    static constexpr void bit_set(T& value) {
        if constexpr (std::is_integral_v<T>) {
            value = SetTo ? T(-1) : T(0);
        } else if constexpr (is_integer_array<T>) {
            for (auto& x : value) {
                bit_set<SetTo>(x);
            }
        } else {
            if (is_constant_evaluated()) {
                using array_type = as_max_size_array<T>;

                auto value_as_arr = bit_cast<array_type>(value);
                bit_set<SetTo>(value_as_arr);
                value = bit_cast<T>(value_as_arr);
                return;
            }
            std::memset(&value, SetTo ? 0xFF : 0, sizeof(T));
        }
    }
    template<class T>
    static constexpr void bit_flip(T& value) {
        if constexpr (is_convertible_as_uint<T>) {
            using uint_t = type_as_uint<T>;
            value = bit_cast<T>(~bit_cast<uint_t>(value));
        } else if constexpr (is_array_that_convertible_as_uint<T>) {
            for (auto& x : value) {
                bit_flip(x);
            }
        } else {
            bit_apply(value, [](auto& x) { bit_flip(x); });
        }
    }
    template<class T>
    static constexpr void bit_flip_at(T& value, const bitsize_t index) {
        if constexpr (std::is_integral_v<T>) {
            value ^= T(1) << index;
        } else if constexpr (std::is_enum_v<T>) {
            auto buffer = static_cast<std::underlying_type_t<T>>(value);
            bit_flip_at(buffer, index);
            value = static_cast<T>(buffer);
        } else if constexpr (is_integer_array<T>) {
            using elem_type = array_value_type<T>;
            const auto array_index = index / (CHAR_BIT * sizeof(elem_type));
            const auto bit_index = index % (CHAR_BIT * sizeof(elem_type));
            bit_flip_at(value[array_index], bit_index);
        } else {
            bit_apply(value, [&](auto& x) { bit_flip_at(x, index); });
        }
    }

    template<class T>
    static constexpr void bit_assign(T& value, const bitsize_t index, const bool what_value) {
        if constexpr (std::is_integral_v<T>) {
            value &= ~(T(1) << index);
            value |= T(what_value) << index;
        } else if constexpr (is_convertible_as_uint<T>) {
            auto buffer = bit_cast<type_as_uint<T>>(value);
            bit_assign(buffer, index, what_value);
            bit_cast_to(value, buffer);
        } else if constexpr (is_array_that_convertible_as_uint<T>) {
            using elem_type = array_value_type<T>;
            const auto array_index = index / (CHAR_BIT * sizeof(elem_type));
            const auto bit_index = index % (CHAR_BIT * sizeof(elem_type));
            bit_assign(value[array_index], bit_index, what_value);
        } else {
            bit_apply(value, [&](auto& x) { return bit_assign(x, index, what_value); });
        }
    }
    template<class T>
    static constexpr bool bit_get(const T& value, const bitsize_t index) {
        if constexpr (is_like_integral<T>) {
            using integer_t = safe_underlying_type<T>;
            return (integer_t(value) & (integer_t(1) << index)) > 0;
        } else if constexpr (is_integer_array<T>) {
            using elem_type = array_value_type<T>;
            const auto array_index = index / (CHAR_BIT * sizeof(elem_type));
            const auto bit_index = index % (CHAR_BIT * sizeof(elem_type));
            return bit_get(value[array_index], bit_index);
        } else {
            using byte_array = std::array<std::byte, sizeof(T)>;
            return bit_get(bit_cast<byte_array>(value), index);
        }
    }

    template<auto* test_function, class T>
    static constexpr bool bit_test_non_trivial_base(const T& value) {
        if constexpr (is_convertible_as_uint<T>) {
            using as_uint_type = type_as_uint<T>;
            return test_function(bit_cast<as_uint_type>(value));
        } else {
            return bit_apply(value, [](const auto& x) { return test_function(x); });
        }
    }

    template<class T>
    static constexpr bool bit_test_all(const T& value) {
        if constexpr (is_convertible_as_uint<T>) {
            using uint_t = type_as_uint<T>;
            return bit_cast<uint_t>(value) == uint_t(-1);
        } else if constexpr (is_array_that_convertible_as_uint<T>) {
            for (const auto x : value) {
                if (bit_test_all(x)) return false;
            }
            return true;
        } else {
            return bit_test_non_trivial_base<&bit_test_all<T>>(value);
        }
    }

    template<class T>
    static constexpr bool bit_test_any(const T& value) {
        if constexpr (is_convertible_as_uint<T>) {
            using uint_t = type_as_uint<T>;
            return bit_cast<uint_t>(value) != uint_t(0);
        } else if constexpr (is_array_that_convertible_as_uint<T>) {
            for (const auto x : value) {
                if (bit_test_any(x)) return true;
            }
            return false;
        } else {
            return bit_test_non_trivial_base<&bit_test_any<T>>(value);
        }
    }
    template<class T>
    static constexpr bool bit_test_none(const T& value) {
        if constexpr (is_convertible_as_uint<T>) {
            using uint_t = type_as_uint<T>;
            return bit_cast<uint_t>(value) == uint_t(0);
        } else if constexpr (is_array_that_convertible_as_uint<T>) {
            for (const auto x : value) {
                if (bit_test_none(x)) return false;
            }
            return true;
        } else {
            return bit_test_non_trivial_base<&bit_test_none<T>>(value);
        }
    }

    template<class What, class... Values>
    static constexpr void bit_emplace(What& what_emplace, const Values&... values) {
        static_assert((sizeof(Values) + ...) == sizeof(value_type));
        std::byte* offset = reinterpret_cast<std::byte*>(std::addressof(what_emplace));

        ((bit_cast_to(*reinterpret_cast<Values*>(offset), values), offset += sizeof(Values)),
         ...);
    }
    template<class What, class T>
    static constexpr void bit_emplace(What& what_emplace, const T& value) {
        static_assert(sizeof(What) == sizeof(T));
        bit_cast_to(what_emplace, value);
    }
    template<class T>
    static constexpr bitsize_t bit_count_for_integer(T value) {
#ifdef __cpp_lib_bitops
        return std::popcount(value_as_uint);
#else
        if (!is_constant_evaluated()) {
    #if defined(_MSC_VER)
            if constexpr (sizeof(T) <= sizeof(unsigned short)) {
                return __popcnt16(value);
            } else if constexpr (sizeof(T) <= sizeof(unsigned int)) {
                return __popcnt(value);
            } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
                return __popcnt64(value);
            }
    #elif BITS_HAS_BUILTIN(__builtin_popcount)
            if constexpr (sizeof(T) <= sizeof(unsigned int)) {
                return __builtin_popcount(value);
            } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
                return __builtin_popcountl(value);
            } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
                return __builtin_popcountll(value);
            }
    #endif
#endif
        }
        bitsize_t count = 0;
        for (; value != 0; ++count) {
            value &= value - 1;
        }
        return count;
    }

    template<class T>
    static constexpr bitsize_t bit_count(const T& value) {
        if constexpr (is_convertible_as_uint<T>) {
            return bit_count_for_integer(bit_cast<type_as_uint<T>>(value));
        } else if constexpr (is_array_that_convertible_as_uint<T>) {
            bitsize_t count = 0;
            for (const auto elem : value) {
                count += bit_count(elem);
            }
            return count;
        } else {
            return bit_apply(value, [](const auto& x) { return bit_count(x); });
        }
    }

    template<class T>
    static constexpr bool is_bits_type = false;
    template<class T>
    static constexpr bool is_bits_type<bits<T>> = true;

    template<class T1, class T2>
    using enable_compare_operator = std::enable_if_t<
        (std::is_same_v<T1, bits> && !is_bits_type<T2>) || std::is_same_v<T2, bits>, int>;

    template<class T>
    constexpr bits& op_assign(const T integer) {
        bit_cast_to(m_value, static_cast<value_as_uint_type>(integer));
        return *this;
    }

    template<class T>
    constexpr auto as_integer_sign_of() const {
        if constexpr (std::is_unsigned_v<T>) {
            return as_uint();
        } else {
            return as_int();
        }
    }
    template<class T>
    constexpr bits& narrow_assign_integer(const T& x) {
        bit_cast_to(m_value, static_cast<value_as_uint_type>(x));
        return *this;
    }

    class narrow_t {
        Value& value;

    public:
        explicit constexpr narrow_t(Value& value) : value(value) {}

        template<class T>
        constexpr void operator=(const T& x) {
            narrow_bit_cast_to(value, x);
        }

        template<class To>
        constexpr To as() const {
            static_assert(!std::is_reference_v<To>);
            To result{};
            narrow_bit_cast_to(result, value);
            return result;
        }
    };

    template<class RefTo>
    class ref_wrapper {
    public:
        constexpr ref_wrapper(Value& value) : value(value) {}

        template<class T>
        constexpr ref_wrapper& operator=(T&& other) {
            static_assert(sizeof(T) == sizeof(RefTo));
            if constexpr (std::is_trivially_assignable_v<RefTo, T>) {
                bit_cast_to(value, other);
            } else {
                RefTo buffer = get();
                buffer = std::forward<T>(other);
                bit_cast_to(value, buffer);
            }
            return *this;
        }
        constexpr RefTo get() const { return bit_cast<RefTo>(value); }
        constexpr operator RefTo() const { return get(); }

        template<class... Args>
        constexpr auto operator()(Args&&... args) const
            -> std::invoke_result_t<RefTo, Args...> {
            return std::invoke(get(), std::forward<Args>(args)...);
        }
        // clang-format off
        template<class T> constexpr auto operator+=(const T& x)
            -> decltype(get() + x, *this) { return *this = get() + x; }
        template<class T> constexpr auto operator-=(const T& x)
            -> decltype(get() - x, *this) { return *this = get() - x; }
        template<class T> constexpr auto operator*=(const T& x)
            -> decltype(get() * x, *this) { return *this = get() * x; }
        template<class T> constexpr auto operator/=(const T& x)
            -> decltype(get() / x, *this) { return *this = get() / x; }
        template<class T> constexpr auto operator%=(const T& x)
            -> decltype(get() % x, *this) { return *this = get() % x; }
        template<class T> constexpr auto operator&=(const T& x)
            -> decltype(get() & x, *this) { return *this = get() & x; }
        template<class T> constexpr auto operator|=(const T& x)
            -> decltype(get() | x, *this) { return *this = get() | x; }
        template<class T> constexpr auto operator^=(const T& x)
            -> decltype(get() ^ x, *this) { return *this = get() ^ x; }
        template<class T> constexpr auto operator<<=(const T& x)
            -> decltype(get() << x, *this) { return *this = get() << x; }
        template<class T> constexpr auto operator>>=(const T& x)
            -> decltype(get() >> x, *this) { return *this = get() >> x; }
        // clang-format on

    private:
        Value& value;
    };

public:
    [[nodiscard]] constexpr narrow_t narrow() { return narrow_t(m_value); }

    class slice {
    private:
        struct none_t {
            constexpr none_t() = default;
        };
        struct some_t {
            constexpr some_t(const bitsize_t value) : value(value) {}
            constexpr some_t() = delete;
            bitsize_t value;
        };

    public:
        explicit constexpr slice(const some_t start, const some_t end)
            : m_start(start.value), m_end(end.value) {}
        explicit constexpr slice(none_t, const bitsize_t end)
            : m_start(static_cast<bitsize_t>(0)), m_end(end) {}
        explicit constexpr slice(const bitsize_t start, none_t)
            : m_start(start), m_end(static_cast<bitsize_t>(-1)) {}

        constexpr bitsize_t start() const { return m_start; }
        constexpr bitsize_t end() const { return m_end; }

    private:
        const bitsize_t m_start;
        const bitsize_t m_end;
    };

    template<class Byte = std::uint_least8_t>
    [[nodiscard]] constexpr auto as_bytes() const {
        static_assert(sizeof(Byte) == sizeof(std::uint_least8_t));
        using byte_array = std::array<Byte, sizeof(value_type) / sizeof(Byte)>;
        return bit_cast<byte_array>(m_value);
    }
    template<class Byte = std::uint_least8_t>
    [[nodiscard]] constexpr auto& as_bytes_ref() {
        static_assert(sizeof(Byte) == sizeof(std::uint_least8_t));
        using byte_array = std::array<Byte, sizeof(value_type) / sizeof(Byte)>;
        return as_ref<byte_array>();
    }

    [[nodiscard]] constexpr auto as_uint() const {
        static_assert(sizeof(value_type) <= sizeof(biggest_uint_t),
                      "Can't be represented as an unsigned integer");
        return bit_cast<value_as_uint_type>(m_value);
    }
    [[nodiscard]] constexpr auto as_int() const {
        static_assert(sizeof(value_type) <= sizeof(biggest_uint_t),
                      "Can't be represented as an signed integer");
        return bit_cast<type_as_int<value_type>>(m_value);
    }

    template<class ArrayType>
    [[nodiscard]] constexpr auto as_array() const {
        static_assert(sizeof(value_type) % sizeof(ArrayType) == 0,
                      "Array type must be a divisible of size of the value_type");
        using array_type = std::array<ArrayType, sizeof(value_type) / sizeof(ArrayType)>;
        return bit_cast<array_type>(m_value);
    }

    template<class To>
    [[nodiscard]] constexpr To as() const {
        static_assert(sizeof(To) == sizeof(value_type));
        static_assert(!std::is_reference_v<To>);
        return bit_cast<To>(m_value);
    }

    template<class To>
    [[nodiscard]] constexpr auto& as_ref() {
        using ref_type = std::conditional_t<std::is_const_v<Value>, const To&, To&>;
        if constexpr (std::is_same_v<value_type, std::remove_const_t<To>>) {
            return static_cast<ref_type>(m_value);
        } else {
            static_assert(sizeof(To) == sizeof(value_type));
            static_assert(!std::is_reference_v<To>);
            return reinterpret_cast<ref_type&>(m_value);
        }
    }

    template<class RefTo>
    [[nodiscard]] constexpr ref_wrapper<RefTo> as_refw() {
        return ref_wrapper<RefTo>(m_value);
    }

    template<class T, std::enable_if_t<!std::is_integral_v<T>, int> = 0>
    constexpr bits& operator=(const T& other) {
        static_assert(sizeof(value_type) == sizeof(T),
            "The size of the assigned type must be equal to the size of the assigner");
        bit_cast_to(m_value, other);
        return *this;
    }
    constexpr bits& operator=(const value_as_uint_type other) {
        bit_cast_to(m_value, other);
        return *this;
    }

    constexpr bitsize_t count() const { return bit_count(m_value); }

    constexpr void set() { bit_set<true>(m_value); }
    constexpr void reset() { bit_set<false>(m_value); }
    constexpr void flip() { bit_flip(m_value); }
    constexpr void flip_at(const bitsize_t index) { bit_flip_at(m_value, index); }

    [[nodiscard]] constexpr bool all() const { return bit_test_all(m_value); }
    [[nodiscard]] constexpr bool any() const { return bit_test_any(m_value); }
    [[nodiscard]] constexpr bool none() const { return bit_test_none(m_value); }

    [[nodiscard]] constexpr auto copy() const { return bit_cast<value_type>(m_value); }

    template<class To>
    constexpr void copy_to(To& to) {
        static_assert(sizeof(To) == sizeof(value_type));
        bit_cast_to(to, m_value);
    }
    template<class To>
    constexpr void copy_to(const To&) = delete;

    template<class... Values>
    constexpr void emplace(const Values&... args) {
        static_assert(
            (sizeof(Values) + ...) == sizeof(value_type),
            "The sum of the sizes of all the types must be equal to the size of the underlying type");
        bit_emplace(m_value, args...);
    }

    // clang-format off
    constexpr bits& operator+=(const value_as_uint_type x) { return op_assign(as_uint() + x); }
    constexpr bits& operator-=(const value_as_uint_type x) { return op_assign(as_uint() - x); }
    constexpr bits& operator*=(const value_as_uint_type x) { return op_assign(as_uint() * x); }
    constexpr bits& operator/=(const value_as_uint_type x) { return op_assign(as_uint() / x); }
    constexpr bits& operator%=(const value_as_uint_type x) { return op_assign(as_uint() % x); }
    constexpr bits& operator&=(const value_as_uint_type x) { return op_assign(as_uint() & x); }
    constexpr bits& operator|=(const value_as_uint_type x) { return op_assign(as_uint() | x); }
    constexpr bits& operator^=(const value_as_uint_type x) { return op_assign(as_uint() ^ x); }
    constexpr bits& operator<<=(const value_as_uint_type x) { return op_assign(as_uint() << x); }
    constexpr bits& operator>>=(const value_as_uint_type x) { return op_assign(as_uint() >> x); }
    // clang-format on

    template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
    friend constexpr bool operator==(const Left& left, const Right& right) {
        static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
                      "The size of the operands is not equal to");
        return bit_compare<std::equal_to<>>(unwrap(left), unwrap(right));
    }
    template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
    friend constexpr bool operator!=(const Left& left, const Right& right) {
        static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
                      "The size of the operands is not equal to");
        return bit_compare<std::not_equal_to<>>(unwrap(left), unwrap(right));
    }
    template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
    friend constexpr bool operator>(const Left& left, const Right& right) {
        static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
                      "The size of the operands is not equal to");
        return bit_compare<std::greater<>>(unwrap(left), unwrap(right));
    }
    template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
    friend constexpr bool operator>=(const Left& left, const Right& right) {
        static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
                      "The size of the operands is not equal to");
        return bit_compare<std::greater_equal<>>(unwrap(left), unwrap(right));
    }
    template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
    friend constexpr bool operator<(const Left& left, const Right& right) {
        static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
                      "The size of the operands is not equal to");
        return bit_compare<std::less<>>(unwrap(left), unwrap(right));
    }
    template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
    friend constexpr bool operator<=(const Left& left, const Right& right) {
        static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
                      "The size of the operands is not equal to");
        return bit_compare<std::less_equal<>>(unwrap(left), unwrap(right));
    }

    class reference {
    public:
        constexpr reference(const bitsize_t index, Value& value)
            : index(index), value(value) {}

        constexpr reference& operator=(const bool x) {
            bit_assign(value, index, x);
            return *this;
        }

        [[nodiscard]] constexpr bool get() const { return bit_get(value, index); }
        [[nodiscard]] constexpr operator bool() const { return get(); }

        constexpr void flip() { bit_flip_at(value, index); }

    private:
        const bitsize_t index;
        Value& value;
    };

    [[nodiscard]] constexpr reference operator[](const bitsize_t index) {
        return reference(index, m_value);
    }

private:
    Value& m_value;
};

template<class T>
bits(T&) -> bits<T>;
template<class T>
bits(const T&) -> bits<const T>;

#undef BITS_ENABLE_INTRINSICS
#undef BITS_HAS_BUILTIN
#undef BITS_HAS_BIT_CAST_INTRINSICS
#undef BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS
