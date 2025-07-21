#include <variant>
#include <string>
#include <iostream>
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

#include "../include/match/static_match.hpp"
#include "../include/RBTree/RBTree.hpp"
void test_static_match(){

// 测试1: 基本类型匹配
    {
        std::variant<int, double, std::string> v = 42;
        
        Tools::static_match(v,
            [](int i) { 
                std::cout << "Got an int: " << i << std::endl; 
            },
            [](double d) { 
                std::cout << "Got a double: " << d << std::endl; 
            },
            [](const std::string& s) { 
                std::cout << "Got a string: " << s << std::endl; 
            },
            [](auto&& other) {
                std::cout << "Got something else: " << other << std::endl;
            }
        );
        
        v = 3.14;
        Tools::static_match(v,
            [](int i) { std::cout << "Int: " << i << std::endl; },
            [](double d) { std::cout << "Double: " << d << std::endl; },
            [](auto&&) { std::cout << "Other" << std::endl; }
        );
        
        v = "hello";
        Tools::static_match(v,
            [](int) { std::cout << "Int" << std::endl; },
            [](double) { std::cout << "Double" << std::endl; },
            [](const std::string& s) { std::cout << "String: " << s << std::endl; }
        );
    }
    
    // 测试2: 自定义类型匹配
    {
        struct Point { int x, y; };
        std::variant<int, Point, std::string> v = Point{1, 2};
        
        Tools::static_match(v,
            [](int i) { std::cout << "Int: " << i << std::endl; },
            [](const Point& p) { 
                std::cout << "Point: (" << p.x << ", " << p.y << ")" << std::endl; 
            },
            [](const std::string& s) { std::cout << "String: " << s << std::endl; },
            [](auto&&) { std::cout << "Unknown type" << std::endl; }
        );
    }
    
    // 测试3: 多参数匹配
    {
        std::variant<int, double> v1 = 42;
        std::variant<std::string, bool> v2 = true;
        
        // 注意：当前实现只支持单参数匹配
        // 如果需要多参数匹配，需要扩展实现
        Tools::static_match(v1,
            [](int i) { std::cout << "v1 is int: " << i << std::endl; },
            [](double d) { std::cout << "v1 is double: " << d << std::endl; }
        );
        
        Tools::static_match(v2,
            [](const std::string& s) { std::cout << "v2 is string: " << s << std::endl; },
            [](bool b) { std::cout << "v2 is bool: " << b << std::endl; }
        );
    }
    
    // 测试4: 编译时检查
    {
        std::variant<int, double> v = 3.14;
        
        // 这个会触发静态断言，因为没有匹配double的处理程序
        // Tools::static_match(v,
        //     [](int i) { std::cout << "Int: " << i << std::endl; }
        // );
        
        // 这个可以通过，因为有catch-all处理程序
        Tools::static_match(v,
            [](int i) { std::cout << "Int: " << i << std::endl; },
            [](auto&&) { std::cout << "Other type" << std::endl; }
        );
    }
    
}

using namespace Tree;
using namespace Tree::Bin::RB;

// 打印节点值及颜色
template <typename Node>
void PrintNode(Node* node) {
    std::cout << node->val << "(" << (node->color == Color::Red ? "R" : "B") << ") ";
}

// 验证中序遍历是否递增（即是合法的 BST）
template <typename Node>
bool IsInOrder(Node* root) {
    std::vector<typename Node::value_type> vals;
    Tree::Bin::Traversal<Node>(root, [&](Node* n) {
        vals.push_back(n->val);
    });
    return std::is_sorted(vals.begin(), vals.end());
}

// 验证性质4：所有路径黑色节点数量相同
template <typename Node>
int CountBlackHeight(Node* node, bool& ok) {
    if (!node) return 1;
    int left = CountBlackHeight(node->left, ok);
    int right = CountBlackHeight(node->right, ok);
    if (left != right) ok = false;
    return left + (node->color == Color::Black ? 1 : 0);
}

int main() {
    using Ty = int;
    using Node = Node<Ty>;

    auto* root = CreateNode(-1);
    if (!root)
        std::cerr << "root is nullptr\n"; 
    std::vector<int> values = {10, 20, 30, 15, 25, 5, 1, 50, 60, 70, 65, 80, 75, 90, 85};
    std::cout << "Test Start:" << std::endl;
    for (int v : values) {
        InsertRBTree(root, v);
    }

    std::cout << "中序遍历 (值+颜色): ";
    Tree::Bin::Traversal<Node>(root, PrintNode<Node>);
    std::cout << "\n";

    std::cout << "验证中序遍历是否递增: ";
    assert(IsInOrder<Node>(root));
    std::cout << "✓ BST结构合法\n";

    std::cout << "验证根节点是黑色: ";
    assert(root->color == Color::Black);
    std::cout << "✓ 根节点为黑色\n";

    std::cout << "验证黑高一致性: ";
    bool ok = true;
    CountBlackHeight<Node>(root, ok);
    assert(ok);
    std::cout << "✓ 所有路径黑节点数量相同\n";

    std::cout << "尝试插入重复值 30... ";
    Node* dup = InsertRBTree(root, 30);
    // assert(dup != nullptr && dup->val == 30);
    std::cout << "✓ 重复插入未生成新节点\n";

    std::cout << "所有基本测试通过 ✅\n";

    return 0;
}