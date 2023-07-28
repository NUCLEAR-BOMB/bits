#include <bits.hpp>
#include <iostream>

int main() {
    float value = 2.f;

    bits{value} += 1 << 22;
    std::cout << value << '\n'; // value == 3

    bits{value} -= 1 << 23;
    std::cout << value << '\n'; // value == 1.5

    bits{value}[31].flip();
    std::cout << value << '\n'; // value == -1.5
}
