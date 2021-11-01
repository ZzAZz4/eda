#include <iostream>
#include "geometry/point.hpp"

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
    for (auto i : p) std::cout << i << ' ';
}