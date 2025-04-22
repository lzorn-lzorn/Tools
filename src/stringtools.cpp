#include "String/stringtools.h"
#include <memory>

namespace _detail {
_string* _fill_string_with_char(const std::string& str, const char c){
    char * _res_str = nullptr;
    _string* res = nullptr;
    size_t n = str.size() * 2 + 1;
    _res_str = std::allocator<char>().allocate(n);
    res = std::allocator<_string>().allocate(1);
    for(size_t i=0; i<n; ++i){
        if (i%2 == 0)
            _res_str[i] = c;
        else
            _res_str[i] = str[(i-1) >> 1];
    }
    res->str = _res_str;
    res->len = n;
    return res;
}
}