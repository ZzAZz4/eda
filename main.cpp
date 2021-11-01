#include <iostream>
#include "geometry/box.hpp"

constexpr Point<int, 3> initialized ()
{
    Point<int, 3> p;
    for (int i = 0; i < (int)std::size(p); ++i)
        p[i] = i;

    return p;
}

int main ()
{
    constexpr auto p = initialized();
    auto p2 = p;
    for (auto i : p2) std::cout << i << ' ';
}