#include <bits.hpp>
#include <iostream>
#include <tuple>

class try_hack_me {
    int ivalue = 2;
    float fvalue = 1e10f;

public:
    void check(int id) const {
        if (ivalue != 2 && fvalue != 1e10f) {
            std::cout << '#' << id << " ivalue=" << ivalue << " fvalue=" << fvalue << '\n';
        } else {
            std::cout << '#' << id << " Failed\n";
        }
    }
};

struct try_hack_me_layout {
    int ivalue;
    float fvalue;
};
void first_way() {
    try_hack_me value_to_hack;
    bits{value_to_hack} = try_hack_me_layout{1, -2.f};
    value_to_hack.check(1);
}
void second_way() {
    try_hack_me value_to_hack;
    auto layout = bits{value_to_hack}.as<try_hack_me_layout>();

    layout.ivalue = 10;
    layout.fvalue = 5.f;

    bits{value_to_hack} = layout;

    value_to_hack.check(2);
}
void third_way() {
    try_hack_me value_to_hack;
    bits{try_hack_me_layout{10, 50.f}}.copy_to(value_to_hack);
    value_to_hack.check(3);
}
void fourth_way() {
    try_hack_me value_to_hack;
    value_to_hack = bits{try_hack_me_layout{-1, -2.f}}.as<try_hack_me>();
    value_to_hack.check(4);
}
void fifth_way() {
    try_hack_me value_to_hack;
    bits{value_to_hack}.emplace<int, float>(123, 456.f);
    value_to_hack.check(5);
}

int main() {
    first_way();
    second_way();
    third_way();
    fourth_way();
    fifth_way();
}
