//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>


int main () {
    using point_type = geom::Point<int, 3>;
    using box_type = geom::Box<point_type>;
    using index_type = index::RTree<int, box_type, 3>;

    index_type tree;

    box_type testbox1({ 1, 2, 3 }, { 1, 2, 3 });
    box_type testbox2({ 2, 3, 4 }, { 2, 3, 4 });
    box_type testbox3({ 3, 4, 5 }, { 3, 4, 5 });
    box_type testbox4({ 4, 5, 6 }, { 4, 5, 6 });
    box_type testbox5({ 5, 6, 7 }, { 5, 6, 7 });

    tree.insert(testbox1, 3);
    tree.insert(testbox2, 4);
    tree.insert(testbox3, 5);

    assert(tree.root->size == 3);
    tree.insert(testbox4, 6);
    assert(tree.root->size == 2);
    tree.insert(testbox5, 7);
    std::cout << "Success\n";
    return 0;
}