#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>

int main ()
{
    geom::tests::box_construct_tests();
    geom::tests::box_intersection_tests();
    geom::tests::box_join_tests();

    index::RTree<int, int, 3, 8, 4> tree;

    geom::Box<int, 3> testbox1({1, 2, 3}, {4, 5, 6});
    geom::Box<int, 3> testbox2({2, 3, 4}, {5, 6, 7});

    tree.insert(testbox1, 3);
    tree.insert(testbox2, 4);

    std::cout << "Success";
}