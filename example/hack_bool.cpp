#include <bits.hpp>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::boolalpha;

    bool value = false;
    std::cout << value << '\n'; // value == false

    bits{value} = 2;
    std::cout << value << '\n'; // value == true
    std::cout << +value << '\n'; // value == 2

    bits{value} = 255;
    std::cout << +value << '\n'; // value == 255
}
