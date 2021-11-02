#include "geometry/ops.hpp"
#include <iostream>

int main ()
{
    geom::tests::box_construct_tests();
    geom::tests::box_intersection_tests();
    geom::tests::box_join_tests();
    std::cout << "Success";
}