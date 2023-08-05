#include <bits.hpp>
#include <iostream>

int main() {
    struct ref_struct {
        int& value;
    };
    int a = 0xABC;
    int b = 0xDEF;

    ref_struct ref{a};

    std::cout << ref.value << '\n'; // ref.value == 2748
    bits{ref} = &b;
    std::cout << ref.value << '\n'; // ref.value == 3567

    bits{ref} = nullptr;
    std::cout << "Oh no!" << std::flush << ref.value << '\n';
}
