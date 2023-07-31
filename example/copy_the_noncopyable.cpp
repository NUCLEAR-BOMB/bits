#include <bits.hpp>
#include <iostream>

struct noncopyable {
    int value;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

int main() {
    noncopyable a{5};

    auto b = bits{a}.copy();

    a.value = 10;
    b->value = 2;
    std::cout << "a: " << a.value << '\n'
              << "b: " << b->value << '\n'
              << '\n'
              << "&a: " << static_cast<void*>(&a) << '\n'
              << "&b: " << static_cast<void*>(&b) << '\n';
}
