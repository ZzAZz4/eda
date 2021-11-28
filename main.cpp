//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include "2015-data/fill_rtree_csv.hpp"
#include <iostream>
#include <sstream>
#include <deque>
#include <fstream>
#include <vector>

// using point_type = geom::Point<int, 3>;
// using box_type = geom::Box<point_type>;
// using index_type = index::RTree<int, box_type, 4>;

using point_type = geom::Point<double, 2>;
using box_type = geom::Box<point_type>;
using index_type = index::RTree<std::string, box_type, 3>;

struct TreePrinter {
    using Tree = index_type;

    struct TextBlock {
        std::deque<std::string> lines;
        std::size_t size{};
    };

    static std::string
    repeat (const std::string& str, unsigned repeats) {
        std::string result;
        while (repeats--) result += str;
        return result;
    }

    static std::string
    center (std::string str, unsigned margin) {
        if (str.size() >= margin) {
            return str;
        }
        size_t pad = margin - str.size();
        size_t lpad = pad / 2;
        size_t rpad = pad - lpad;
        str = repeat(" ", lpad) + std::move(str) + repeat(" ", rpad);
        return str;
    }

    static std::string
    point_as_text (const typename Tree::point_type& point) {
        std::ostringstream os;
        os << '(';
        std::string sep;
        for (unsigned i = 0; i < point.size(); ++i) {
            os << sep << point[i];
            sep = " ";
        }
        os << ')';
        return os.str();
    }

    static std::string box_as_text (const typename Tree::box_type& box) {
        std::ostringstream os;
        os << '[';
        os << point_as_text(box.lower);
        os << point_as_text(box.upper);
        os << ']';
        return os.str();
    }

    static TextBlock
    node_as_text (typename Tree::const_node_pointer node) {
        if (node->_c_is_leaf) {
            return node_as_text(
                static_cast<typename Tree::const_leaf_pointer>(node));
        }
        else {
            return node_as_text(
                static_cast<typename Tree::const_inner_pointer>(node));
        }
    }

    static TextBlock
    node_as_text (typename Tree::const_leaf_pointer node) {
        std::ostringstream up_os;
        std::ostringstream lo_os;
        std::string sep;
        up_os << "|";
        lo_os << "|";

        for (unsigned i = 0; i < node->size; ++i) {
            std::ostringstream temp;
            temp << node->_records[i];
            auto lo_str = temp.str();
            auto up_str = box_as_text(node->_boxes[i]);

            up_str = center(up_str, lo_str.size());
            lo_str = center(lo_str, up_str.size());

            up_os << sep << up_str;
            lo_os << sep << lo_str;
            sep = " ";
        }

        up_os << "|";
        lo_os << "|";
        auto up_str = up_os.str();
        auto lo_str = lo_os.str();
        auto size = up_str.size();
        return {{ up_str, lo_str }, size };
    }

    static TextBlock
    node_as_text (typename Tree::const_inner_pointer node) {
        std::vector<TextBlock> below_blocks;
        TextBlock answer;
        std::ostringstream up_os;
        std::string sep;

        for (unsigned i = 0; i < node->size; ++i) {
            auto block = node_as_text(node->_records[i]);
            below_blocks.push_back(block);
        }

        auto rows = below_blocks[0].lines.size();
        for (size_t row = 0; row < rows; ++row) {
            std::ostringstream lo_os;
            sep = "";
            for (const auto& block : below_blocks) {
                lo_os << sep << block.lines[row];
                sep = " ";
            }
            auto line = lo_os.str();
            answer.lines.push_back(line);
            answer.size = std::max(answer.size, line.size());
        }

        up_os << "|";
        sep = "";
        for (unsigned i = 0; i < node->size; ++i) {
            auto up_str = box_as_text(node->_boxes[i]);
            up_os << sep << up_str;
            sep = " ";
        }
        up_os << "|";
        auto up_str = up_os.str();
        up_str = center(up_str, answer.size);
        answer.lines.push_front("");
        answer.lines.push_front(up_str);
        return answer;
    }

    static std::string
    stringify (const Tree& tree) {
        TextBlock block = node_as_text(tree.root);
        std::string result;
        for (const auto& line : block.lines) {
            result += line;
            result += '\n';
        }
        return result;
    }
};

// int main () {

//     index_type tree;
//     int element_count = 400;

//     for (int i = 0; i < element_count; ++i) {
//         point_type point{i, i + 1, i + 2};
//         box_type box{point, point};
//         tree.insert(box, (int) i + 3);
//     }

//     box_type query_box({ 273, 274, 275 }, { 275, 276, 277 });
//     std::vector<int> result;
//     tree.query(query_box, std::back_inserter(result));
//     for (auto i : result) std::cout << i << ' ';
//     std::cout << '\n';

//     std::ofstream test("tree_pprint.txt");
//     test << TreePrinter::stringify(tree);

//     return 0;
// }
int main () {

    index_type tree;
    rtree_filler(tree);

    box_type query_box({ 273, 274 }, { 275, 276 });
    std::vector<std::string> result;
    tree.query(query_box, std::back_inserter(result));
    for (auto i : result) std::cout << i << ' ';
    std::cout << '\n';

    std::ofstream test("tree_pprint.txt");
    test << TreePrinter::stringify(tree);

    return 0;
}