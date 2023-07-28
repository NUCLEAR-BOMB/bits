#define BITS_COMPILER_INTRINSICS 1
#include <bits.hpp>
#include <iostream>

struct noncopyable {
    int value;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

int main() {
    noncopyable a{5};

    noncopyable b = bits{a}.copy();

    a.value = 10;
    b.value = 2;
    std::cout << "a: " << a.value << ", b: " << b.value << '\n';
}
