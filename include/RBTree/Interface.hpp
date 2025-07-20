#pragma once

#include <memory>
#include <functional>
template <typename Ty, class Alloc=std::allocator<Ty>>
struct ITreeNode {
public: 
    using value_type      = Ty;
    using node_type       = ITreeNode<Ty, Alloc>;
    using allocator_type  = Alloc;
    using alloc_traits    = std::allocator_traits<Alloc>;
    using pointer         = node_type*;
    using const_pointer   = const node_type*;
    using reference       = node_type&;
    using const_reference = const node_type&;
public:
    virtual ~ITreeNode() = 0;
    virtual const value_type& GetValue() const = 0;
};

template <typename Ty, class Alloc=std::allocator<Ty>>
class ITree{
public:
    using value_type      = Ty;
    using allocator_type  = Alloc;
    using node_type       = ITreeNode<Ty, Alloc>;
    using pointer         = node_type*;
    using const_pointer   = const node_type*;
    using reference       = node_type&;
    using const_reference = const node_type&;
    using visitor         = std::function<void(pointer)>;
public:
    virtual ~ITree() = 0;

    virtual pointer Insert(value_type&& value) = 0;
    virtual pointer Remove(const value_type& value) = 0;
    virtual pointer Find(const value_type& value) const = 0;

    /// 中序遍历，传入根节点和访问函数
    virtual void Traverse(const_pointer root, visitor visit) const = 0;

    /// 若支持分配器访问
    virtual allocator_type GetAllocator() const = 0;
};