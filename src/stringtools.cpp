#include "String/stringtools.h"
#include <memory>
namespace _detail {
std::string _fill_string_with_char(const std::string& str, const char c){
    size_t n = str.size() * 2 + 1;
    auto res = std::string();

    for (size_t i = 0; i < n; ++i) {
        res += (i % 2 == 0) ? c : str[(i - 1) / 2];
    }
    return res;
}

std::string _remove_sp_char_for_manacher(const std::string _str, int center, int radius){
    std::string result;
    // 预处理后的回文子串起始和结束索引
    size_t start_pre = center - radius + 1;
    size_t end_pre = center + radius - 1;

    for (size_t i = start_pre; i <= end_pre; ++i) {
        if (i % 2 != 0) {  // 仅保留原字符（奇数索引）
            result += _str[i];
        }
    }
    return result;
}

std::string _manacher(const std::string& s){
    if (s.empty()) return std::string();

    auto processed_str = _fill_string_with_char(s, '#');
    size_t n = processed_str.size();
    std::vector<int> radius_array(n, 0);  // 使用 vector 自动管理内存[6](@ref)

    int center = 0, max_radius = 0;
    int current_center = 0, current_right = 0;

    for (int i = 0; i < n; ++i) {
        int mirror = 2 * current_center - i;
        int expand = (i < current_right) ? std::min(radius_array[mirror], current_right - i) : 1;

        // 中心扩展
        while (i - expand >= 0 && i + expand < n &&
               processed_str[i - expand] == processed_str[i + expand]) {
            expand++;
        }

        radius_array[i] = expand;
        if (i + expand > current_right) {  // 更新边界[2,4](@ref)
            current_center = i;
            current_right = i + expand;
        }

        if (expand > max_radius) {
            max_radius = expand;
            center = i;
        }
    }

    return _remove_sp_char_for_manacher(processed_str, center, max_radius);
}
}