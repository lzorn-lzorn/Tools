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
        : val(val), parent (parent), left(left), right(right), color(Color::Black) {}

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

    template <typename Ty>
    inline static Node<Ty>* Minimum(Node<Ty>* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }
    /*
     * function: 用一棵子树 v 替换另一棵子树 u 在树中的位置
     * Eg: Transplant(B, D) 把 D 接到 B 的位置，并把 D->parent = A
     *        A               A
     *       / \             / \
     *      B   C    ==>    D   C
     *     / \             / \
     *    x   D           x   E
     *       / \
     *      E   F
     *  
     */
    template <typename Ty>
    inline static void Transplant(Node<Ty>*& root, Node<Ty>* u, Node<Ty>* v) {
        if (!u->parent)
            root = v; // u 是根节点，替换 root 本身
        else if (u == u->parent->left)
            u->parent->left = v; // u 是左孩子，用 v 替换
        else
            u->parent->right = v; // u 是右孩子，用 v 替换
        if (v)
            v->parent = u->parent; // 设置 v 的新父节点
    }


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



    /* 插入方法的四种情况: 插入 Z 节点
    * case 1: Z == root
    * case 2: Z.uncle = red
    * case 3: Z.uncle = black (triangle)
    * case 4: Z.uncle = black (line)
    */
    template <typename Ty>
    inline static Node<Ty>* InsertRBTree(Node<Ty>*& root, const Ty&& val){
        Node<Ty>* node = CreateNode(val);
        node->color = Color::Red;
        InsertBinTree(root, node);
        // Fix: 处理以上四种情况
    }

    template <typename Ty>
    inline static Node<Ty>* InsertBinTree(Node<Ty>*& root, Node<Ty>* node){
        if (!root){
            root = node;
            root->color = Color::Black;
            return node;
        }
        // if (node == root){
        //     node->color = Color::Black;
        //     return node;
        // }
        Node<Ty>* current = root;
        Node<Ty>* parent = nullptr;
        while (current) {
            parent = current;
            if (node->value < current->value) {
                current = current->left;
            } else if (node->value > current->value) {
                current = current->right;
            } else {
                // 重复值，不插入，直接返回已有节点（也可以允许插入）
                return current;
            }
        }
        node->parent = parent;
        if (node->value < parent->value) {
            parent->left = node;
        } else {
            parent->right = node;
        }

        return node;
    }

    // @return true: 删除成功
    // @return false: 删除失败(不存在节点)
    template <typename Ty>
    inline static Node<Ty>* RemoveRBTree(Node<Ty>*& root, const Ty val){
        Node<Ty>* target = Find(root, val);
        if (!target){
            return nullptr;
        }
        RemoveBinTree(root, val);
        // Fix: 
    }

    template <typename Ty>
    inline static bool RemoveBinTree(Node<Ty>*& root, const Ty& val) {
        Node<Ty>* z = FindBinTree(root, val);
        if (!z) return false;

        if (!z->left) {
            Transplant(root, z, z->right);
        } else if (!z->right) {
            Transplant(root, z, z->left);
        } else {
            Node<Ty>* y = Minimum(z->right); // 找中序后继
            if (y->parent != z) {
                Transplant(root, y, y->right);
                y->right = z->right;
                if (y->right) y->right->parent = y;
            }
            Transplant(root, z, y);
            y->left = z->left;
            if (y->left) y->left->parent = y;
        }

        delete z; // 假设你手动管理节点
        return true;
    }

    // @return nullptr: 没找到
    template <typename Ty>
    inline static Node<Ty>* Find(const Node<Ty>* const root, const Ty& val){
        while (root) {
            if (val < root->value)
                root = root->left;
            else if (val > root->value)
                root = root->right;
            else
                return root;
        }
        return nullptr;
    }

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