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

    GraphInstance& UpdateVertex(const uint64_t id, VerTy val){
        _m_vertexs[id] = val;
    }

    GraphInstance& AddEdge(const uint64_t from_id, const uint64_t to_id, WeightType weight=WeightType(1)){
        // * 检查点 from_id, to_id 是否存在, 不存在则初始化
        uint64_t _id = std::max(from_id, to_id);
        if (_m_vertexs.size() < _id){
            _m_vertexs.resize(_id+1, std::nullopt);
            _m_vertexs[_id] = VerTy();
        }else{
            _m_vertexs[from_id] ?
                (_m_vertexs[to_id] ?  : _m_vertexs[to_id]=VerTy()) : _m_vertexs[from_id]=VerTy();
        }
        _m_edges.try_emplace({from_id, to_id}, weight);
        return *this;
    }
    GraphInstance& UpdateEdge(const std::pair<uint64_t, uint64_t> idx, WeightType weight=WeightType(1)){
        return _m_edges[idx] = weight, *this;
    }
    GraphInstance& UpdateEdge(const uint64_t from_id, const uint64_t to_id, WeightType weight=WeightType(1)){
        return _m_edges[{from_id, to_id}] = weight, *this;
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
    std::unordered_map<std::pair<const uint64_t, const uint64_t>, WeightType> _m_edges;
};


template <typename VerTy = int, typename WeightType = double>
class GraphManager final{
private:
    struct GraphAllocInfo{
        using Allocator = std::allocator<GraphInstance<VerTy, WeightType>>;
        std::unique_ptr<
            GraphInstance<VerTy, WeightType>,
            std::function<void(GraphInstance<VerTy, WeightType>*)>
        > _ptr;
        bool flag = false; // * 该内存是否初始化
    };
    using Allocator = typename GraphAllocInfo::Allocator;
public:
    GraphManager<VerTy, WeightType>& Instance() {
        static GraphManager<VerTy, WeightType> _m_instance;
        return _m_instance;
    }
    GraphManager(const GraphManager&) = delete;
    GraphManager& operator=(const GraphManager&) = delete;

    GraphInstance<VerTy,  WeightType>& CreateGraph() {
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
                    info._ptr.reset(ptr, [allocator](auto* p) {
                        std::destroy_at(allocator, p);
                        allocator.deallocate(p, 1);
                    });
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
                new_info._ptr.reset(ptr, [allocator](auto* p) {
                    std::destroy_at(p);
                    allocator.deallocate(p, 1);
                });
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
    void DestoryGraph(const uint64_t id){
        std::lock_guard<std::mutex> lock(_mutex);
        if (id >= _m_graphs.size()) return;

        auto& info = _m_graphs[id];
        if (!info.flag) return;

        std::destroy_at(info._ptr.get()); // * 销毁对象
        info.flag = false;
    }

    void Destory(){
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
                info._ptr.reset(ptr, [allocator](auto* p) {
                    std::destroy_at(allocator, p);
                    allocator.deallocate(p, 1);
                });
                info.flag = false;
                _m_graphs.push_back(std::move(info));
            } catch (const std::bad_alloc& e) {
                std::cerr << "Initial allocation failed: " << e.what() << std::endl;
            }
        }
    }
    ~GraphManager() = default;
private:
    std::vector<GraphAllocInfo> _m_graphs;
    mutable std::mutex _mutex;
};

}