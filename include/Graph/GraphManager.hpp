#pragma once
#include <cstdint>
#include <memory>
#include <algorithm>
#include <new>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <functional>
#include <optional>
#include "Graph/GraphNode.hpp"
#include "GraphNode.hpp"
namespace Moonlight::Graph {
/*
 * 图的本质只是点的集合和边的集合
 */
template <typename VerTy = int, typename WeightType = double>
class GraphInstance{
public:
    GraphInstance() = default;

    GraphInstance(GraphInstance<VerTy, WeightType>&& other){
        if (*this == other){
            return;
        }
        id = other.id;
        start_id = other.start_id;
        _m_list = std::move(other._m_list);
        _m_matrix = std::move(other._m_matrix);
        _m_edges = std::move(other._m_edges);
        _m_vertexs = std::move(other._m_edges);
    }
    GraphInstance& operator=(GraphInstance<VerTy, WeightType>&& other){
        if (*this == other){
            return;
        }
        id = other.id;
        start_id = other.start_id;
        _m_list = std::move(other._m_list);
        _m_matrix = std::move(other._m_matrix);
        _m_edges = std::move(other._m_edges);
        _m_vertexs = std::move(other._m_edges);
        return *this;
    }
    void Destory(){
        if (_m_list){
            _m_list->Destory();
            _m_list = nullptr;
        }
        if (_m_matrix){
            _m_matrix->Destory();
            _m_matrix = nullptr;
        }
        _m_vertexs.clear();
        _m_edges.clear();
    }
public:
    GraphInstance& AddVertex(VerTy val=VerTy()){
        _m_vertexs.push_back(val);
        return *this;
    }
    GraphInstance& AddNVertex(const uint64_t n, VerTy val=VerTy()){
        for (uint64_t i=0; i<n; ++i){
            _m_vertexs.push_back(val);
        }
        return *this;
    }

    template <typename ...Args>
    GraphInstance& AddNVertex(Args... args){
        (_m_vertexs.pop_back(args), ...);
        return *this;
    }
    GraphInstance& UpdateVertex(const uint64_t id, VerTy val){
        _m_vertexs[id] = val;
    }

    GraphInstance& AddEdge(const uint64_t from_id, uint64_t to_id, WeightType weight=WeightType(1)){
        // * 检查点 from_id, to_id 是否存在, 不存在则初始化
        uint64_t _id = std::max(from_id, to_id);
        if (_m_vertexs.size() < _id){
            _m_vertexs.resize(_id+1, std::nullopt);
            _m_vertexs[_id] = VerTy();
        }else{
            _m_vertexs[from_id] ?
                (_m_vertexs[to_id] ?  : _m_vertexs[to_id]=VerTy()) : _m_vertexs[from_id]=VerTy();
        }

        _m_edges.insert({.from=from_id, .to=to_id, .weight=weight});
        return *this;
    }

    GraphInstance& UpdateEdge(uint64_t from_id, uint64_t to_id, WeightType weight=WeightType(1)){
        auto it = _m_edges.find({.from=from_id, .to=to_id});
        if (it != _m_edges.end()){
            (*it).weight = weight;
        }
        return *this;
    }
    AdjacencyList<VerTy, WeightType>& List() const {
        if (!_m_list){
            _m_list = AdjacencyList(_m_edges);
        }
        return &_m_list;
    }
    AdjacencyMatrix<VerTy, WeightType>& Matrix() const {
        if (!_m_matrix){
            _m_matrix = AdjacencyMatrix(_m_vertexs.size(), _m_edges);
        }
        return &_m_matrix;
    }

private:
    uint64_t id;
    uint64_t start_id{1}; // * 起点顶点的id
    std::unique_ptr<AdjacencyList<VerTy, WeightType>> _m_list {nullptr};
    std::unique_ptr<AdjacencyMatrix<VerTy, WeightType>> _m_matrix {nullptr};

    std::vector<std::optional<VerTy>> _m_vertexs;
    std::unordered_set<
                        Edge<WeightType>,
                        typename Edge<WeightType>::EdgeHash,
                        typename Edge<WeightType>::EdgeEqual
                      > _m_edges;
};


template <typename VerTy = int, typename WeightType = double>
class GraphManager final{
private:
    struct GraphAllocInfo{
        using Allocator = std::allocator<GraphInstance<VerTy, WeightType>>;
        std::unique_ptr<
            GraphInstance<VerTy, WeightType>,
            std::function<void(GraphInstance<VerTy, WeightType>*)> // 自定义内存删除器
        > _ptr;
        bool flag = false; // * 该内存是否初始化
    };
    using Allocator = typename GraphAllocInfo::Allocator;
public:
    static GraphManager<VerTy, WeightType>& Instance() {
        static GraphManager<VerTy, WeightType> _m_instance;
        return _m_instance;
    }
    GraphManager(const GraphManager&) = delete;
    GraphManager& operator=(const GraphManager&) = delete;

    GraphInstance<VerTy,  WeightType>& GetAEmptyGraph() {
        std::lock_guard<std::mutex> lock(_mutex);

        // 查找第一个可用的内存块
        auto it = std::find_if(_m_graphs.begin(), _m_graphs.end(),
            [](const GraphAllocInfo& info) { return !info.flag; });

        if (it != _m_graphs.end()){
            Allocator allocator;
            auto& info = *it;
            if (!info._ptr) { // 内存未分配
                try {
                    auto* ptr = allocator.allocate(1);
                    std::construct_at(ptr);
                    info._ptr = std::unique_ptr<
                                    GraphInstance<VerTy, WeightType>,
                                    std::function<void(GraphInstance<VerTy, WeightType>*)>
                                >(
                                    ptr,
                                    [allocator](GraphInstance<VerTy, WeightType>* p) mutable {
                                        std::destroy_at(p);         // 先析构对象
                                        allocator.deallocate(p, 1); // 再释放内存
                                    }
                                );
                } catch (const std::bad_alloc& e) {
                    std::cerr << "Allocation failed: " << e.what() << std::endl;
                    throw;
                }
            } else { // 内存已分配但未初始化
                std::construct_at(info._ptr.get());
            }
            info.flag = true;
            return *info._ptr;
        }

        // 扩容逻辑
        Allocator allocator;
        std::vector<GraphAllocInfo> new_allocs;
        try {
            for (size_t i = 0; i < 2; ++i) {
                auto* ptr = allocator.allocate(1);
                std::construct_at(ptr);
                GraphAllocInfo new_info;
                new_info._ptr = std::unique_ptr<
                                    GraphInstance<VerTy, WeightType>,
                                    std::function<void(GraphInstance<VerTy, WeightType>*)>
                                >(
                                    ptr,
                                    [allocator](GraphInstance<VerTy, WeightType>* p) mutable {
                                        std::destroy_at(p);         // 先析构对象
                                        allocator.deallocate(p, 1); // 再释放内存
                                    }
                                );
                new_info.flag = true;
                new_allocs.push_back(std::move(new_info));
            }
        } catch (...) {
            for(auto& info: new_allocs){
                if (info._ptr){
                    std::destroy_at(info._ptr.get());
                    info._ptr.reset();
                }
            }
            throw;
        }

        // 将新分配的内存加入管理
        _m_graphs.reserve(_m_graphs.size() + new_allocs.size());
        for (auto& info : new_allocs) {
            _m_graphs.push_back(std::move(info));
        }
        return *_m_graphs[_m_graphs.size() - 2]._ptr; // 返回第一个新分配的元素
    }
    void DestoryAGraph(const uint64_t id){
        std::lock_guard<std::mutex> lock(_mutex);
        if (id >= _m_graphs.size()) return;

        auto& info = _m_graphs[id];
        if (!info.flag) return;

        std::destroy_at(info._ptr.get()); // * 销毁对象
        info.flag = false;
    }

    void Destory(){
        if (!this->_b_state){
            return ;
        }
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto& info : _m_graphs) {
            if (info._ptr) {
                std::destroy_at(info._ptr.get());
                info._ptr.reset();
            }
        }
        _m_graphs.clear();
    }

    GraphInstance<VerTy,  WeightType>& GetGraph(const uint64_t id) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (id >= _m_graphs.size() || !_m_graphs[id].flag) {
            throw std::out_of_range("Invalid graph ID");
        }
        return *_m_graphs[id]._ptr;
    }
    const GraphInstance<VerTy,  WeightType>& GetGraph(const uint64_t id) const {
        std::lock_guard<std::mutex> lock(_mutex);
        if (id >= _m_graphs.size() || !_m_graphs[id].flag) {
            throw std::out_of_range("Invalid graph ID");
        }
        return *_m_graphs[id]._ptr;
    }
private:
    GraphManager(){
        Allocator allocator;
        _m_graphs.reserve(2);
        for (size_t i = 0; i < 2; ++i) {
            try {
                auto* ptr = allocator.allocate(1);
                std::construct_at(ptr);
                GraphAllocInfo info;
                info._ptr = std::unique_ptr<
                                GraphInstance<VerTy, WeightType>,
                                std::function<void(GraphInstance<VerTy, WeightType>*)>
                            >(
                                ptr,
                                [allocator](GraphInstance<VerTy, WeightType>* p) mutable {
                                    std::destroy_at(p);         // 先析构对象
                                    allocator.deallocate(p, 1); // 再释放内存
                                }
                            );
                info.flag = false;
                _m_graphs.push_back(std::move(info));
            } catch (const std::bad_alloc& e) {
                std::cerr << "Initial allocation failed: " << e.what() << std::endl;
            }
        }
        this->_b_state = false;
    }
    ~GraphManager(){
        this->Destory();
    }
private:
    std::vector<GraphAllocInfo> _m_graphs;
    bool _b_state{false}; // 是否被释放
    mutable std::mutex _mutex;
};

}