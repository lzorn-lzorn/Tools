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
#include <type_traits>
#include <concepts>
#include <functional>
#include <iostream>

namespace RBTreeTools {
    
    enum class Position {
        Root = -1, Left = 0, Right = 1
    };
    enum class Color {
        Red = 0, Black = 1
    };

    constexpr bool operator!(Color c) noexcept{
        return c == Color::Red;    
    }
    template <typename NodePtr, typename Compare = std::less<>>
    concept TreeNode = requires(NodePtr node, std::remove_pointer_t<NodePtr>::value_type val, Compare comp){
        // 检查成员存在且为指针类型
        { node.parent } -> std::convertible_to<std::add_pointer_t<NodePtr>>;
        { node.left } -> std::convertible_to<std::add_pointer_t<NodePtr>>;
        { node.right } -> std::convertible_to<std::add_pointer_t<NodePtr>>;
        { node.val };
        { typename std::remove_pointer_t<NodePtr>::value_type };
        { typename std::remove_pointer_t<NodePtr>::node_type };
        // 确保不是方法而是成员变量
        requires std::is_member_object_pointer_v<decltype(&NodePtr::parent)>;
        requires std::is_member_object_pointer_v<decltype(&NodePtr::left)>;
        requires std::is_member_object_pointer_v<decltype(&NodePtr::right)>;
        requires std::is_member_object_pointer_v<decltype(&NodePtr::val)>;
        // 内部或外部存在比较器
        requires requires {
            { val < node->val } -> std::convertible_to<bool>;
        } || requires {
            { comp(val, node->val) } -> std::convertible_to<bool>;
        };
    };

    
    template <typename NodePtr>
    concept RBTreeNode = requires(NodePtr node){
        // 继承基础树节点约束
        requires TreeNode<NodePtr, typename NodePtr::value_type>;  
        // 检查颜色成员（允许bool/enum/自定义类型）
        { node.color } -> std::convertible_to<Color>;
        // 确保是成员变量
        requires std::is_member_object_pointer_v<decltype(&NodePtr::color)>;
        requires requires { 
            { node.color = Color::Red } -> std::same_as<Color&>; // 可写
            { const_cast<const NodePtr>(node).color } -> std::same_as<Color>; // 可读 
        };
    };

    template <RBTreeNode NodeTp,
                typename Ty, 
                class Alloc = std::allocator<typename NodeTp::node_type>>
    inline static NodeTp* Nil() {
        using NodePtr = NodeTp*;
        using AllocTraits = std::allocator_traits<Alloc>;
        // comment: 懒汉单例
        static NodePtr nil = [] {
            Alloc alloc;
            NodePtr ptr = AllocTraits::allocate(alloc, 1);
            std::construct_at(ptr, Ty{}, nullptr, nullptr, nullptr);
            return ptr;
        }();

        return nil;
    }


    template <typename Ty>
    struct RBNode {
        using value_type = Ty;
        using node_type = RBNode<Ty>;
        Ty val;
        Color color;
        node_type * parent;
        node_type * left;
        node_type * right;

        explicit RBNode()
            : val(Ty{}), parent(Nil()), left(Nil()), right(Nil()), color(Color::Black) {}

        explicit RBNode (Ty val, node_type *parent=Nil(), node_type *left=Nil(), node_type *right=Nil()) 
            : val(val), parent (parent), left(left), right(right), color(Color::Black) {}

        bool operator<(const RBNode& elem) const {
            return this->val < elem.val;
        }
        bool operator==(const RBNode& elem) const {
            return this->val == elem.val;
        }
    };

    template <typename Ty, RBTreeNode NodeTp=RBNode<Ty>>
    inline static NodeTp* CreateNode(
        Ty&& val
    ) {
        using NodePtr = NodeTp;
        using Alloc = std::allocator<NodePtr>;
        using AllocTraits = std::allocator_traits<Alloc>;

        Alloc alloc;
        NodePtr nil = Nil();
        NodePtr* addr = AllocTraits::allocate(alloc, 1);
        std::construct_at(addr, std::forward<Ty>(val), nil, nil, nil);
        return addr;
    }
    // template <typename Ty>
    // inline static Node<Ty>* CreateNode(Ty&& val) {
    //     return CreateNode<Node, std::remove_cvref_t<Ty>>(std::forward<Ty>(val));
    // }
    template <RBTreeNode NodeTp, 
                typename Ty, 
                class Alloc=std::allocator<Ty>>
    static NodeTp* DestoryNode(NodeTp* node) {
        std::destroy_at(node);
        return node;
    }

    template <RBTreeNode NodeTp, 
                typename Ty, 
                class Alloc=std::allocator<typename NodeTp::node_type>>
    static NodeTp* DestroyRBTree(NodeTp* node) {
        if (node->left) [[likely]] {
            DestoryNode(node->left);
        }
        if (node->right) [[likely]] {
            DestoryNode(node->right);
        }
        DestoryNode(node);
    }

    // @return nullptr: 没找到
    template <TreeNode NodeTp, typename Ty=NodeTp::value_type>
    inline static NodeTp* Find(NodeTp* root, const Ty& val){
    #ifndef NDEBUG
        static_assert(std::same_as<typename NodeTp::value_type, Ty>, "Find: 查找类型不一致");
    #endif
        NodeTp* current = root;
        
        while (current) {
            if (val < current->val)
                current = current->left;
            else if (val > current->val)
                current = current->right;
            else
                return current;
        }
        return nullptr;
    }
    template <TreeNode NodeTp>
    inline static bool IsLeft(NodeTp* parent, NodeTp* child){
        if (!parent || !child) [[unlikely]] {
            return false;
        }
        return parent->left == child;
    }

    template <TreeNode NodeTp>
    inline static bool IsRight(NodeTp* parent, NodeTp* child){
        if (!parent || !child) [[unlikely]] {
            return false;
        }
        return parent->right == child;
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
    template <TreeNode NodeTp>
    inline static void Transplant(NodeTp*& root, NodeTp* u, NodeTp* v) {
        if (!u->parent)
            root = v; // u 是根节点，替换 root 本身
        else if (u == u->parent->left)
            u->parent->left = v; // u 是左孩子，用 v 替换
        else
            u->parent->right = v; // u 是右孩子，用 v 替换
        if (v)
            v->parent = u->parent; // 设置 v 的新父节点
        u->parent = v->parent;
    }

    /*
    * @function: 该接口要求 node != nullptr 
    */
    template <TreeNode NodeTp>
    inline static Position Which(NodeTp* node){
        if (!node->parent){
            return Position::Root;
        }
        if (node->parent->left == node){
            return Position::Left;
        }
        if (node->parent->right == node){
            return Position::Right;
        }
    }

    template <TreeNode NodeTp>
    inline static NodeTp* Uncle(NodeTp* node){
        if (!node || !node->parent || !node->parent->parent){
            return nullptr;
        }
        if (Which(node->parent) == Position::Left){
            return node->parent->parent->right;
        } else if (Which(node->parent) == Position::Right) {
            return node->parent->parent->left;
        } else{
            return nullptr;
        }
    }

    template <TreeNode NodeTp>
    inline static NodeTp* Minimum(NodeTp* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }

    template <TreeNode NodeTp, typename Ty=NodeTp::value_type>
    inline static NodeTp* RemoveBinTree(NodeTp*& root, const Ty& val) {
        NodeTp* z = Find(root, val);
        if (!z) return false;

        if (!z->left) {
            Transplant(root, z, z->right);
        } else if (!z->right) {
            Transplant(root, z, z->left);
        } else {
            NodeTp* y = Minimum(z->right); // 找中序后继
            if (y->parent != z) {
                Transplant(root, y, y->right);
                y->right = z->right;
                if (y->right) y->right->parent = y;
            }
            Transplant(root, z, y);
            y->left = z->left;
            if (y->left) y->left->parent = y;
        }

        return z;
    }

    template <typename NodeTp>
    inline static NodeTp* InsertBinTree(NodeTp*& root, NodeTp* node){
        using NodePtr = NodeTp*;
        if (!root){
            root = node;
            return node;
        }
        // if (node == root){
        //     node->color = Color::Black;
        //     return node;
        // }
        NodePtr nil  = Nil();
        NodePtr current = root;
        NodePtr parent = nullptr;
        while (current != nil) {
            parent = current;
            if (node->val < current->val) {
                current = current->left;
            } else if (node->val > current->val) {
                current = current->right;
            } else {
                // 重复值，不插入，直接返回已有节点（也可以允许插入）
                return current;
            }
        }
        node->parent = parent;
        if (!parent)
            std::cerr << "parent is nullptr\n";
        if (!node)
            std::cerr << "node is nullptr\n";
        if (node->val < parent->val) {
            parent->left = node;
        } else {
            parent->right = node;
        }

        return node;
    }
    template <TreeNode NodeTp>
    inline static void Traversal(NodeTp* root, std::function<void(NodeTp*)> visit){
        using NodePtr = NodeTp*;
        std::stack<NodePtr> stack;
        NodePtr current = root;
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

    template <TreeNode NodeTp, typename Ty>
    inline static void Traversal(const NodeTp* root, std::function<void(NodeTp*)> visit){
        std::stack<NodeTp*> stack;
        NodeTp* current = root;
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
    template <RBTreeNode NodeTp>
    static void LeftRotate(NodeTp*& root, NodeTp* x) {
        if (!x || !x->right) [[unlikely]] return;

        NodeTp* y = x->right;
        NodeTp* B = y->left;

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
    template <RBTreeNode NodeTp>
    static void RightRotate(NodeTp*& root, NodeTp* x) {
        if (!x || !x->left) [[unlikely]] return;

        NodeTp* y = x->left;
        NodeTp* B = y->right;

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

    /*
        *       B           B   
        *     /  \        /  \  
        *    C    A  or  A   C  => true
        *        /       \ 
        *      Z          Z    
    */
    template <RBTreeNode NodeTp>
    inline static bool IsTriangle(NodeTp* Z, NodeTp* A, NodeTp* B){
        if (!Z || !A || !B || A == B || A == Z || Z == B){
            return false;
        }
        return (IsLeft(A, Z) && IsRight(B, A)) || IsRight(A, Z) && IsLeft(B, A);
    }

    /*
        *       B           B   
        *     /  \        /  \  
        *    C    A  or  A   C  => true
        *         \     /     
        *         Z    Z    
    */
    template <RBTreeNode NodeTp>
    inline static bool IsLine(NodeTp* Z, NodeTp* A, NodeTp* B){
        if (!Z || !A || !B || A == B || A == Z || Z == B){
            return false;
        }
        return (IsRight(A, Z) && IsRight(B, A)) || (IsLeft(A, Z) && IsLeft(B, A));
    }

    
    /* 插入方法的四种情况: 插入 Z 节点
    * case 1: Z == root
    * case 2: Z.uncle = red
    * case 3: Z.uncle = black (triangle)
    * case 4: Z.uncle = black (line)
    */ 
    template <RBTreeNode NodeTp, typename Ty=NodeTp::value_type>
    inline static NodeTp* InsertRBTree(NodeTp*& root, Ty&& val){
    #ifndef NDEBUG
        static_assert(std::same_as<typename NodeTp::value_type, Ty>, 
            "InsertRBTree: Insert Type must be consistent with value_type of RBNode");
    #endif

        using NodePtr = NodeTp*;
        NodePtr node = CreateNode(std::forward<Ty>(val)); // 完美转发
        node->color = Color::Red;

        NodePtr& raw_root = reinterpret_cast<NodePtr&>(root);  // 强制统一类型
        NodePtr res = InsertBinTree(root, node);

        while (res != root && res->parent->color == Color::Red) {
            if (Which(res->parent) == Position::Left) {
                NodePtr uncle = res->parent->parent->right;
                if (uncle && uncle->color == Color::Red) {
                    // Case 1: uncle is red
                    res->parent->color = Color::Black;
                    uncle->color = Color::Black;
                    res->parent->parent->color = Color::Red;
                    res = res->parent->parent;
                } else {
                    if (Which(res) == Position::Right) {
                        // Case 2: triangle
                        res = res->parent;
                        LeftRotate(root, res);
                    }
                    // Case 3: line
                    res->parent->color = Color::Black;
                    res->parent->parent->color = Color::Red;
                    RightRotate(root, res->parent->parent);
                }
            } else {
                // Mirror version
                NodePtr uncle = res->parent->parent->left;
                if (uncle && uncle->color == Color::Red) {
                    res->parent->color = Color::Black;
                    uncle->color = Color::Black;
                    res->parent->parent->color = Color::Red;
                    res = res->parent->parent;
                } else {
                    if (Which(res) == Position::Left) {
                        res = res->parent;
                        RightRotate(root, res);
                    }
                    res->parent->color = Color::Black;
                    res->parent->parent->color = Color::Red;
                    LeftRotate(root, res->parent->parent);
                }
            }
        }
        root->color = Color::Black;
        return res;
    }

    // @return true: 删除成功
    // @return false: 删除失败(不存在节点)
    template <RBTreeNode NodeTp, typename Ty>
    inline static NodeTp* RemoveRBTree(NodeTp*& root, const Ty val){
    #ifndef NDEBUG
        static_assert(std::same_as<typename NodeTp::value_type, Ty>, 
            "RemoveRBTree: Remove Type must be consistent with value_type of RBNode");
    #endif
        NodeTp* target = Find(root, val);
        if (!target){
            return nullptr;
        }
        RemoveBinTree(root, val);
        // Fix: 
    }            

 // Bin
} // namesapce Tree   
template <typename Ty>
struct RBTree {
public:
    using value_type = Ty;
    using node_type = RBTreeTools::RBNode<Ty>;

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
    node_type* head { nullptr };
};
