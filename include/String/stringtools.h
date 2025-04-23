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

// @function: 将str中按照c字符进行分割, 返回一个新的字符串
// @param: str 处理字符串
// @param: c   用于分割的字符
// @param: count 分割的次数, 默认为-1即为全部分割
// @return: 字符串的数组
static std::vector<std::string> Split(const std::string& str, const char c, const size_t count=-1);

// @function: 在str中使用 src_str 替换 dst_str, 此替换不超过count次, 默认-1全部替换, 返回一个新字符串
static std::string Replace(const std::string& str, const std::string src_str, const std::string dst_str, const size_t count);

// @function: 在str中使用 src_str 替换 dst_str, 此替换不超过count次, 默认-1全部替换, 直接在原字符串上操作
static std::string Replace(std::string& str, const std::string src_str, const std::string dst_str);