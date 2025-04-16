#pragma once
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <utility>
#include <vector>
#include <cstdint>

namespace Moonlight::Graph {
template<typename Weight=double>
struct Edge{
public:
    Edge() = default;
    Edge(const Edge&)  = default;
    Edge(Edge&&)  = default;
    Edge(uint64_t from, uint64_t to, Weight weight=static_cast<Weight>(1))
    : from(from), to(to), weight(weight){}
public:
    struct EdgeHash{
        using is_transparent = void;  // 启用透明哈希
        size_t operator()(const Edge& e) const {
            return _hash(e.from, e.to);
        }
        size_t operator()(const std::pair<uint64_t, uint64_t>& p) const {
            return _hash(p.first, p.second);;
        }
        size_t operator()(const uint64_t from, const uint64_t to) const {
            return _hash(from, to);
        }
    private:
        size_t _hash(const uint64_t from, const uint64_t to) {
            return std::hash<uint64_t>{}(from) * 31 + std::hash<uint64_t>{}(to);
        }
    };


    struct EdgeEqual{
        bool operator()(const Edge& e1, const Edge& e2) const {
            return (e1.from == e2.from && e1.to == e2.to) ||
               (e1.from == e2.to && e1.to == e2.from);
        }
    };

public:
    uint64_t from, to;
    Weight weight;
};

template <typename VerTy=int, typename Weight=double>
struct AdjacencyList{
    AdjacencyList() = default;
    // * 边的集合初始化
    explicit AdjacencyList(std::unordered_set<Edge<Weight>> list) {

    }
    void Destory(){
        _m_list.clear();
    }

public:
    template <typename WeightType=Weight>
    AdjacencyList& SetEdge(const uint64_t from, const uint64_t to, WeightType weight){

        return *this;
    }
    AdjacencyList& AddVertex(){

    }
    void SetStartVertex(const uint64_t id){
        start_id = id;
    }
    uint64_t Id() const {
        return id;
    }
private:
    // * 邻接表是from->to, 其值为权重
    std::vector<std::vector<Weight>> _m_list;
    // * 起点默认设置为uint64_t start_id = 1 的点
    uint64_t start_id{1};
    // * 邻接表本身的id
    uint64_t id;
};

template <typename VerTy=int, typename Weight=double>
struct AdjacencyMatrix{
    AdjacencyMatrix() = default;
    AdjacencyMatrix(const uint64_t number,
                    std::unordered_set<
                        Edge<Weight>,
                        typename Edge<Weight>::EdgeHash,
                        typename Edge<Weight>::EdgeEqual
                    > edges){}

    void Destory(){
        _m_matrix.clear();
    }
private:
    std::vector<Weight> _m_matrix;
};
}