#pragma once
#include <vector>
#include <string>

namespace _detail {
    struct _string{
        char* str;
        size_t len;
    };
    // @function: 判断 str 是否是一个回文串
    bool _is_palindrome(const std::string& str);

    // @function: 使用一个字符 c, 其填充字符串 str
    _string* _fill_string_with_char(const std::string& str, const char c='#');
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
static std::vector<std::string> GetMaxSubPalindrome(const std::string str);