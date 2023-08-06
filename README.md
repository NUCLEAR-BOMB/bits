# About

bits is a library for direct bitwise value manipulation.

## Features

- Single class library
- Abstraction over bits
- Almost without reinterpret_cast
- Allows enable the C++20 compiler extension (`BITS_COMPILER_INTRINSICS`)
- Constexpr support (integers, trivial types and some other cases)
- Safe bitwise conversions (unless you don't want it)
- Similar to `std::bitset`

## Functions

List of `bits` class methods:

- `as_bytes()`: returns `std::array` of `std::uint_least8_t` with bits of the current value
- `as_bytes_ref()`: reinterpret_cast to `std::array` of `std::uint_least8_t`
- `as_array<T>()`: returns `std::array` of `T` with bits of the current value
- `as_uint()`: returns an unsigned integer with bits of the current value
- `as_int()`: returns an signed integer with bits of the current value
- `as<T>()`: bitwise cast to type `T`
- `as_ref<T>()`: reinterpret_cast to type `T&`
- `as_refw<RefTo>()`: returns a reference wrapper around `T&` that uses `memcpy`/`bit_cast`. Methods:
    - `operator=(T&&)`: bitwise assignment
    - `get()`: returns a bitwise converted `RefTo`
    - `operator RefTo()`: same as `get()`
    - `operator()(Args&&... args)`: invokes `RefTo`
    - `operator{+,-,*,/,%,&,|,^,<<,>>}=`: applies the value as `RefTo`
- `operator=(const T&)`: bitwise assignment
- `operator=(unsigned integer)`: overload `operator=`, which makes it easy to assign bits as an integer
- `count()`: returns the number of bits that are set to `true`
- `set()`: set all bits to `true`
- `reset()`: set all bits to `false`
- `flip()`: flips all the bits
- `flip_at(index)`: flips the bit in the `index` position
- `all()`: checks if all of the bits are set to `true`
- `any()`: checks if any of the bits are set to `true`
- `none()`: checks if none of the bits are set to `true`
- `copy()`: bitwise copies to the current type. If the copy constructor is deleted returns a wrapper
- `copy_to(T&)`: bitwise copies to the `to` variable
- `emplace(const Values&...)`: bitwise inserts `Values...` in place of the current value
- `operator{+,-,*,/,%,&,|,^,<<,>>}=(unsigned integer)`: applies the operation to the current value as an unsigned integer
- `operator{==,!=,>,>=,<,<=}`: bitwise compares the current value and with another passed value
- `operator[](index)`: returns the bit proxy `bits::reference` to the `index` position. `bits::reference` methods:
    - `operator=(bool)`: assigns a bit to the `index` position
    - `get()`: returns the bit as `bool`
    - `operator bool()`: same as `get()`
    - `flip()`: flips the bit at the `index` position
- `operator()(first, last)`: returns a bit slice `bits::slice` in the range of [`first`, `last`). `bits::slice` methods:
    - `first()`: returns the beginning of slice in index
    - `last()`: returns the end of the slice in the index
    - `operator=(unsigned integer)`: assigns bits in the slice range and ignores the remain bits. Supports only types that can fit into an unsigned integer
- `narrow()`: returns an object that provides narrowing and expanding operations on bits. `bits:narrow_t` methods:
    - `operator=(const T&)`: bitwise assignment without size restriction
    - `as<T>()`: bitwise cast to type `T` without size restriction

## Examples

[hack_private.cpp](example/hack_private.cpp):
Modifying a private class field

[hack_bool.cpp](example/hack_bool.cpp):
Modifying the value of bool beyond 0 and 1

[float_to_int.cpp](example/float_to_int.cpp):
Modifying the float value as an integer

[copy_the_noncopyable.cpp](example/copy_the_noncopyable.cpp):
Creating a copy of a class without a copy constructor

[modify_reference.cpp](example/modify_reference.cpp):
Modification of a C++ reference as a structure field

## Building with CMake

Add to CMakeLists.txt:
```cmake
include(FetchContent)
FetchContent_Declare(bits
    GIT_REPOSITORY https://github.com/NUCLEAR-BOMB/bits.git
    # Specify the commit hash that you would like to use the library with
    GIT_TAG 03a61c7750487ad272b04cf96d66468e539c8309
)
FetchContent_MakeAvailable(bits)

add_executable(my_target main.cpp)
target_link_libraries(my_target bits)
```

## CMake Options

- `BITS_CLANG_FORMAT` (default: `PROJECT_IS_TOP_LEVEL`)
    - Adds the clang-format target 'format'
- `BITS_BUILD_TEST` (default: `PROJECT_IS_TOP_LEVEL`)
    - Enable bits testing. Adds a target 'bits-test'
- `BITS_BUILD_EXAMPLE` (default: `PROJECT_IS_TOP_LEVEL`)
    - Enable bits example. Adds targets with the name
    'example-{name}'
- `BITS_SANITIZER` (default: `TRUE`)
    - Enables an AddressSanitizer in MSVC or an AddressSanitizer with UndefinedBehaviorSanitizer in other compilers
- `BITS_CLANG_TIDY` (default: `TRUE`)
    - Adds the clang-tidy target 'tidy' for Makefile and Ninja generators
- `BITS_GTEST_BRIEF` (default: `FALSE`)
    - Enables gtest brief mode like running test program with --gtest_brief=1 or by setting the GTEST_BRIEF environment variable to 1
- `BITS_GTEST_DEBUG_BREAK` (default: `FALSE`)
    - Enables gtest break_on_failure and catch_exceptions=0 like running test program with --gtest_break_on_failure and --gtest_catch_exceptions=0 or by setting the GTEST_BREAK_ON_FAILURE environment variable to a value other than 0 and setting the GTEST_CATCH_EXCEPTIONS environment variable to 0
