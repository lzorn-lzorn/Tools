#pragma once
#include <cstdint>
#include <list>
#include <type_traits>
#include <utility>
#include <vector>
#include <concepts>
#include <optional>
namespace Moonlight::Graph {

template <typename VerTy>
struct Vertex;

template<typename VerTy>
struct VertexPool{
public:
    VertexPool() = default;
    explicit VertexPool(const uint64_t size){
        _m_pool.reserve(size);
        std::fill(_m_pool.begin(), _m_pool.end(), std::nullopt);
    }

public:
    Vertex<VerTy>& operator[](const uint64_t id){
        if (id > _m_pool.size()){
            _m_pool.resize(id+1);
        }
        return &_m_pool[id];
    }
    void SetVertex(const uint64_t id, const VerTy data){
        if (id > _m_pool.size()){
            _m_pool.resize(id+1);
        }
        _m_pool[id] = data;
    }
    std::vector<VerTy> GetPool() const {
        return _m_pool;
    }
    uint64_t GetIdleId() const {
        uint64_t _n = _m_pool.size();
        for (uint64_t i=0; i<_n; ++i){
            if (_m_pool[i].data == std::nullopt){
                return i;
            }
        }
        _m_pool.resize(_n+1);
        return _n;
    }
    VertexPool& Push(const VerTy& vertex){
        return _m_pool.push_back(vertex), *this;
    }
    VertexPool& Remove(const uint64_t id){
        return _m_pool.erase(id), *this;
    }
    VertexPool& Pop(){
        return _m_pool.pop_back(), *this;
    }
private:
    std::vector<VerTy> _m_pool;
};


template <typename VerTy>
struct Vertex{
public:
    explicit Vertex () : data(VerTy()) {}
    explicit Vertex (const uint64_t id, const VerTy data) : _m_id(id), data(data){}
    ~Vertex() {}
    Vertex(const Vertex& ver) = default;
    Vertex& operator=(const Vertex& ver) = default;
    Vertex(Vertex&&) = delete;
    Vertex& operator=(Vertex&&) = delete;
public:
    uint64_t GetId() const {
        return _m_id;
    }

public:
    std::optional<VerTy> data{std::nullopt};
private:
    uint64_t _m_id{0};
};
template <typename VerTy, typename Weight=double>
struct Edge{
    Vertex<VerTy> from, to;
    Weight weight;
};


template <typename VerTy, typename Weight=double>
struct AdjacencyList{
    AdjacencyList() = default;
    AdjacencyList(std::vector<Edge<Weight, VerTy>> list): _m_pool(2*list.size()){
        _m_list.reserve(list.size());
        for(auto begin=list.begin(); begin != list.end(); ++begin){
            auto _id = (*begin).from;
            
        }
    }
    AdjacencyList(std::vector<Vertex<VerTy>> list): _m_pool(list.size()){
        _m_list.reserve(list.size());
        _m_pool = list;
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
private:
    VertexPool<VerTy> _m_pool;
    // * 邻接表是from->to
    std::vector<std::list<std::pair<uint64_t,Weight>>> _m_list;
};

template <typename VerTy, typename Weight=double>
struct AdjacencyMatrix{

private:
    std::vector<Weight> _m_matrix;
};
}