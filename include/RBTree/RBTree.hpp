#pragma once 
/*
 *  红黑树的规则:
 *  1. 所有节点不是红的就是黑的
 *  2. 根结点是黑的
 *  3. 红节点的孩子一定是黑的
 *  4. 所有路径上黑色节点的数量一样多
 *  5. 所有叶子节点都是黑的 (NIL)
 */

#include <memory>
#include <stack>
#include <utility>
#include <functional>

namespace RBTree {

enum class Color {
    Red = 0, Black = 1
};

constexpr bool operator!(Color c) noexcept{
    return c == Color::Red;    
}


template <typename Ty>
struct Node {
    using value_type = Ty;
    using node_type = Node<Ty>;
    Ty val;
    Color color;
    node_type * parent;
    node_type * left;
    node_type * right;

    explicit Node()
        : val(Ty{}), parent(nullptr), left(nullptr), right(nullptr), color(Color::Black) {}

    explicit Node (Ty val, node_type *parent=nullptr, node_type *left=nullptr, node_type *right=nullptr) 
        : val(val), parent (parent), left(left), right(right), color(Color::Black){}

    bool operator<(const Node& elem) const {
        return this->val < elem.val;
    }
    bool operator==(const Node& elem) const {
        return this->val == elem.val;
    }
};

namespace tools {
    template <typename Ty, class Alloc=std::allocator<Node<Ty>>>
    static Node<Ty>* CreateNode(
        Ty&& val,
        Node<Ty>* parent=nullptr,
        Node<Ty>* left=nullptr,
        Node<Ty>* right=nullptr
    ) {
        using Node = Node<Ty>;
        using AllocTraits = std::allocator_traits<Alloc>;

        Alloc alloc;
        Node* addr = AllocTraits::allocate(alloc, 1);
        std::construct_at(addr, std::forward<Ty>(val), parent, left, right);
        return addr;
    }
    template <typename Ty, class Alloc = std::allocator<Node<Ty>>>
    static Node<Ty>* GetNil() {
        using Node = Node<Ty>;
        using AllocTraits = std::allocator_traits<Alloc>;
        // comment: 懒汉单例
        static Node* nil = [] {
            Alloc alloc;
            Node* ptr = AllocTraits::allocate(alloc, 1);
            std::construct_at(ptr);
            return ptr;
        }();

        return nil;
    }
    template <typename Ty, class Alloc=std::allocator<Ty>>
    static Node<Ty>* DestoryNode(Node<Ty>* node) {
        std::destroy_at(node);
        return node;
    }

    template <typename Ty, class Alloc=std::allocator<Ty>>
    static Node<Ty>* DestoryRBTree(Node<Ty>* node) {
        if (node->left) [[likely]] {
            DestoryNode(node->left);
        }
        if (node->right) [[likely]] {
            DestoryNode(node->right);
        }
        DestoryNode(node);
    }

   /*
    * 左旋操作：将节点 x 向左旋转，使其右子节点 y 成为新的父节点
    * 图示（左旋）：
    *
    *   Before:
    *         P                  P
    *         |                  |
    *         x                  y
    *        / \                / \
    *       A  y     ===>      x   γ
    *         / \             / \    
    *        B   γ           A   B
    */
    template <typename Node>
    static void LeftRotate(Node*& root, Node* x) {
        if (!x || !x->right) [[unlikely]] return;

        Node* y = x->right;
        Node* B = y->left;

        // 左旋核心结构变换
        y->left = x;
        x->right = B;

        if (B)
            B->parent = x;

        y->parent = x->parent;

        if (!x->parent) {
            root = y;
        } else if (x->parent->left == x) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        x->parent = y;
    }

    /*
    * 右旋操作：将节点 x 向右旋转，使其左子节点 y 成为新的父节点
    * 图示（右旋）：
    *
    *   Before:             After:
    *        P                  P
    *        |                  |
    *        x                  y
    *       / \                / \    
    *      y   γ     ===>     α   x
    *     / \                    / \
    *    α   B                  B   γ
    *
    */
    template <typename Node>
    static void RightRotate(Node*& root, Node* x) {
        if (!x || !x->left) [[unlikely]] return;

        Node* y = x->left;
        Node* B = y->right;

        // 右旋核心结构变换
        y->right = x;
        x->left = B;

        if (B)
            B->parent = x;

        y->parent = x->parent;

        if (!x->parent) {
            root = y;
        } else if (x->parent->right == x) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }

        x->parent = y;
    }
    template <typename Node>
    static void Fix(Node* node);


template <typename Ty>
inline static void Traversal(const Node<Ty>* root, std::function<void(Node<Ty>*)> visit){
    std::stack<Node<Ty>*> stack;
    Node<Ty> * current = root;
    while(current || !stack.empty()){
        while(current){
            stack.push(current);
            current = current->left;
        }
        
        current = stack.top();
        stack.pop();

        visit(current);

        current = current->right;
    }
}


template <typename Ty>
inline static void Insert(const Node<Ty>* const root, Node<Ty>* node);

// @return true: 删除成功
// @return false: 删除失败(不存在节点)
template <typename Ty>
inline static bool Remove(const Node<Ty>* const root, Node<Ty>* node);

// @return nullptr: 没找到
template <typename Ty>
inline static Node<Ty>* Find(const Node<Ty>* const root, Node<Ty>* node);

} // namespace tools
template <typename Ty>
struct RBTree {
public:
    using value_type = Ty;
    using node_type = Node<Ty>;

public:
    RBTree() : head(nullptr) {}
    RBTree(node_type *head) : head(head) {} 

    void SetRoot(node_type * root) noexcept {
        if (root == nullptr) [[unlikely]] {
            return ;
        }
        head = root;
    }
private:
    node_type head { nullptr };
};
}