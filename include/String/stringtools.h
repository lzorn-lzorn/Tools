#pragma once
#include <vector>
#include <string>
#include <memory>

namespace _detail {
    // @function: 判断 str 是否是一个回文串
    bool _is_palindrome(const std::string& str);

    // @function: 使用一个字符 c, 其填充字符串 str
    std::string _fill_string_with_char(const std::string& str, const char c='#');
    std::string _remove_sp_char_for_manacher(const std::string _str, int32_t center, int32_t radius);
    std::string _manacher(const std::string&);

}

static bool IsPalindrome(const std::string& str){
    if (str.size() == 0) return true;
    size_t n = str.size();
    size_t i=0, j=n-1;

    while(i < j){
        if(str[i] != str[j]){
            return false;
        }
        ++i, --j;
    }
    return true;
}

// @function: 获得字串 str 中所有的回文子串
// @note: 使用的算法是 Manacher 算法,
// @note: 其时间复杂度是 O(n), 其中 n = str.size()
static std::string GetMaxSubPalindrome(const std::string str){
    return _detail::_manacher(str);
}