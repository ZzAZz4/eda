#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>

int main ()
{
    using point_type = geom::Point<int, 3>;
    using box_type = geom::Box<point_type>;

    index::RTree<int, box_type, 8, 4> tree;

    geom::Box<point_type> testbox1({1, 2, 3}, {4, 5, 6});
    geom::Box<point_type> testbox2({2, 3, 4}, {5, 6, 7});

    tree.insert(testbox1, 3);
    tree.insert(testbox2, 4);

    std::cout << "Success";
}