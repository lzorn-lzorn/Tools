#pragma once
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

    const std::vector<_Ty> operator[](const size_t i) const {}
    std::vector<_Ty> operator[](const size_t i) {}

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
    void RemovePositionOf(const size_t x, const size_t y) {
        _p_arr[_p_translate_to_index(x, y)] = std::nullopt;
    }
    void RemoveValueOf(const _Ty val, const size_t num=1){}
    void Insert(const size_t x, const size_t y, _Ty val) {}
    void Insert(_Ty val) {}

private:
    std::array<std::optional<value_type>, size> _p_arr;
    struct coordinate{
        size_t x, y;
    };

private:
    coordinate _p_translate_to_coordinate(const int64_t index){
        coordinate _tmp_coordinate;
        _tmp_coordinate.x = index % N;
        _tmp_coordinate.y = index / N;
        return _tmp_coordinate;
    }
    // N * (y-1) + x 但是x, y是从零开始数的
    const int64_t _p_translate_to_index(const int32_t x, const int32_t y){
        return N * y + x;
    }
};