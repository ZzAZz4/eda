//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>
#include <sstream>
#include <deque>


using point_type = geom::Point<int, 4>;
using box_type = geom::Box<point_type>;
using index_type = index::RTree<int, box_type, 3>;

struct TreePrinter {
    using Tree = index::RTree<int, box_type, 3>;

    struct TextBlock {
        std::deque<std::string> lines;
        std::size_t size;
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
        return {{ up_str, lo_str }, up_str.size() };
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

int main () {

    index_type tree;

    box_type testbox1({ 1, 2, 3 }, { 1, 2, 3 });
    box_type testbox2({ 2, 3, 4 }, { 2, 3, 4 });
    box_type testbox3({ 3, 4, 5 }, { 3, 4, 5 });
    box_type testbox4({ 4, 5, 6 }, { 4, 5, 6 });
//    box_type testbox5({ 5, 6, 7 }, { 5, 6, 7 });

    tree.insert(testbox1, 3);
    tree.insert(testbox2, 4);
    tree.insert(testbox3, 5);
    tree.insert(testbox4, 6);
//    tree.insert(testbox5, 7);

    box_type query_box({ 1, 2, 3 }, { 4, 5, 6 });
    std::vector<int> result;
    tree.query(query_box, std::back_inserter(result));
    for (auto i : result) std::cout << i << ' ';
    std::cout << '\n';

    std::cout << TreePrinter::stringify(tree);

    return 0;
}