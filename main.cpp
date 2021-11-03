#include "index/rtree.hpp"
#include <iostream>

int main ()
{
    geom::tests::box_construct_tests();
    geom::tests::box_intersection_tests();
    geom::tests::box_join_tests();

    geom::Box<int, 3> testbox({1, 2, 3}, {4, 5, 6});
    index::RTree<int, int, 3, 8, 4> tree;
    tree.insert(testbox, 3);
    tree.insert(testbox, 4);
    std::cout << "Success";
}