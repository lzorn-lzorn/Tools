#pragma once
#include <unordered_map>
#include <vector>
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
            this->_init_list();
        }
        return &_m_list;
    }
    AdjacencyMatrix<VerTy, WeightType>& Matrix() const {
        if (!_m_matrix){
            this->_init_matrix();
        }
        return &_m_matrix;
    }
private:
    void _init_list() {}
    void _init_matrix() {}
private:
    uint64_t id;
    uint64_t start_id{1}; // * 起点顶点的id
    AdjacencyList<VerTy, WeightType>* _m_list {nullptr};
    AdjacencyMatrix<VerTy, WeightType>* _m_matrix {nullptr};

    std::vector<std::optional<VerTy>> _m_vertexs;
    std::unordered_map<std::pair<const uint64_t, const uint64_t>, WeightType> _m_edges;
};


template <typename VerTy = int, typename WeightType = double>
class GraphManager final{
public:
    GraphManager<VerTy, WeightType>& Instance() {
        static GraphManager<VerTy, WeightType> _m_instance;
        return _m_instance;
    }
    GraphManager(const GraphManager&) = delete;
    GraphManager& operator=(const GraphManager&) = delete;

    GraphInstance<VerTy,  WeightType>& Graph() {
        GraphInstance instance;
        return instance;
    }
private:
    GraphManager() = default;
    ~GraphManager() = default;
private:
    std::vector<GraphInstance<VerTy, WeightType>> _m_graphs;
};

}