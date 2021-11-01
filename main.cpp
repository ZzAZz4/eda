#include "geometry/ops.hpp"
#include <iostream>

int main ()
{
    geom::detail::box_construct_tests();
    geom::detail::box_intersect_tests();
    std::cout << "Success";
}