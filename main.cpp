#include "geometry/ops.hpp"
#include <iostream>

int main ()
{
    geom::detail::box_construct_tests();
    geom::detail::box_intersection_tests();
    std::cout << "Success";
}