#pragma once
#include <functional>
#include <array>
#include <optional>
#include <vector>
template <typename _Ty, size_t N, size_t M>
class Array2D{
public:
    using value_type = _Ty;

    constexpr static uint64_t size = N * M;
    constexpr static size_t row_num = M;
    constexpr static size_t column_num = N;

public:
    Array2D(){
        std::fill(_p_arr.begin(), _p_arr.end(), std::nullopt);
    }
    Array2D(std::array<std::array<_Ty, N>, M>& arr2d){
        for (const auto arr : arr2d){
            for(const auto val : arr){
                _p_arr.push_back(std::optional<_Ty>(val));
            }
        }
    }
    Array2D(std::vector<std::vector<_Ty>>& arr2d){
        for (const auto arr : arr2d){
            for(const auto val : arr){
                _p_arr.push_back(std::optional<_Ty>(val));
            }
        }
    }

  const _Ty at(const size_t i, const size_t j) const {}
    _Ty at(const size_t i, const size_t j) {} 

    const std::vector<_Ty> find(const _Ty val) const {}
    const uint64_t find_first_of(const _Ty val) const {}

    void Append(_Ty value) {
        if(_p_arr.size() + 1 > size){
            return ;
        }
        _p_arr.push_back(std::optional<_Ty>(value));
    }
    // @function: 将 (x, y) 处的元素设置为空
    void RemovePositionAt(const size_t x, const size_t y) {
        _p_arr[_p_translate_to_index(x, y)] = std::nullopt;
    }
    /*
    * @function: 从 (x, y) 处开始, 保留前 reserve 个元素, 删除后 num 个元素
    * @param: val, 待删除的值
    * @param: (x, y) 从哪个位置开始
    * @param: reserve 保留前n个元素, reserve <= 0 时, 则不会保留
    * @param: num 删除后 num 个元素, 
    *   - num <= 0 时, 则不会删除; 
    *   - 当 num 大于当前矩阵中持有的 val 个数时(除前reserve), 则全部删除;
    * @return: 该函数只保证执行之后二维数组中不存在 val, 不做任何返回
    * @note: 该算法的(x, y)指定的位置最优先, 即无论(x, y)之前有没有val, 该算法都会无视
    * @note：即从(x, y)之后才开始执行Remove
    */  
    void Remove(const _Ty& val, 
                const size_t x=0, const size_t y=0,
                const size_t reserve=0, 
                const size_t num=1){}

    void Remove(const _Ty val, 
                const size_t x=0, const size_t y=0,
                const size_t reserve=0, 
                const size_t num=1){}
    void Insert(const size_t x, const size_t y, _Ty val) {}
    void Insert(_Ty val) {
        this->Append(val);
    }
 
    bool Find(const _Ty& val, const size_t& x, const size_t& y) noexcept {}

    /*
    * @function: 查找第一个符合谓词 Pred 的元素
    * @param: (x, y) 是算法最后返回的位置;
    * @param: condition 是一个谓词, 要求最后返回一个bool表示是否满足条件
    * @param: args... 是额外 condition 需要的参数包
    * @return: 如果找到结果返回 true, 否则返回 false
    */
    template <typename Pred = std::function<bool(_Ty, ...)>, typename ...Args>
    bool FindIf(const size_t&x, const size_t& y, Pred condition, Args&... args) {}

private:
    std::array<std::optional<value_type>, size> _p_arr;
    struct coordinate{
        size_t x, y;
    };

private:
    coordinate _p_index_to_xy(const int64_t index){
        coordinate _tmp_coordinate;
        _tmp_coordinate.x = index % N;
        _tmp_coordinate.y = index / N;
        return _tmp_coordinate;
    }
    // N * (y-1) + x 但是x, y是从零开始数的
    const int64_t _p_xy_to_index(const int32_t x, const int32_t y){
        return N * y + x;
    }
};