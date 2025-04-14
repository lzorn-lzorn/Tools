#pragma once
#include <cstdint>
#include <list>
#include <unordered_map>
#include <utility>
#include <vector>
#include <concepts>
#include <optional>
namespace Moonlight::Graph {


template <typename VerTy=int, typename Weight=double>
struct AdjacencyList{
    AdjacencyList() = default;
    // * 边的集合初始化
    explicit AdjacencyList(std::unordered_map<std::pair<const uint64_t, const uint64_t>, Weight> list) {

    }
    void Destory(){
        _m_list.clear();
    }
public:
    template <typename WeightType=Weight>
    AdjacencyList& SetEdge(const uint64_t from, const uint64_t to, WeightType weight){
        const uint64_t size = _m_list[from].size();
        auto begin = _m_list[from].begin();
        auto end = _m_list[from].end();
        for (; begin != end; ++begin){
            if (begin.first == to){
                begin.second = weight;
                break;
            }
        }
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
    // * 邻接表是from->to
    std::vector<std::list<std::pair<uint64_t,Weight>>> _m_list;
    // * 起点默认设置为uint64_t start_id = 1 的点
    uint64_t start_id{1};
    // * 邻接表本身的id
    uint64_t id;
};

template <typename VerTy=int, typename Weight=double>
struct AdjacencyMatrix{
    AdjacencyMatrix() = default;
    AdjacencyMatrix(const uint64_t number, std::unordered_map<std::pair<const uint64_t, const uint64_t>, Weight> edges){}

    void Destory(){
        _m_matrix.clear();
    }
private:
    std::vector<Weight> _m_matrix;
};
}