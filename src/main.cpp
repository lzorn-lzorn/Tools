// --- Test Example ---
#include <variant>
#include <string>
#include <iostream>

#include "../include/match/static_match.hpp"
int main() {
    // 测试1: 基本类型匹配
    {
        std::variant<int, double, std::string> v = 42;
        
        Tools::static_match(v,
            [](int i) { 
                std::cout << "Got an int: " << i << std::endl; 
            },
            [](double d) { 
                std::cout << "Got a double: " << d << std::endl; 
            },
            [](const std::string& s) { 
                std::cout << "Got a string: " << s << std::endl; 
            },
            [](auto&& other) {
                std::cout << "Got something else: " << other << std::endl;
            }
        );
        
        v = 3.14;
        Tools::static_match(v,
            [](int i) { std::cout << "Int: " << i << std::endl; },
            [](double d) { std::cout << "Double: " << d << std::endl; },
            [](auto&&) { std::cout << "Other" << std::endl; }
        );
        
        v = "hello";
        Tools::static_match(v,
            [](int) { std::cout << "Int" << std::endl; },
            [](double) { std::cout << "Double" << std::endl; },
            [](const std::string& s) { std::cout << "String: " << s << std::endl; }
        );
    }
    
    // 测试2: 自定义类型匹配
    {
        struct Point { int x, y; };
        std::variant<int, Point, std::string> v = Point{1, 2};
        
        Tools::static_match(v,
            [](int i) { std::cout << "Int: " << i << std::endl; },
            [](const Point& p) { 
                std::cout << "Point: (" << p.x << ", " << p.y << ")" << std::endl; 
            },
            [](const std::string& s) { std::cout << "String: " << s << std::endl; },
            [](auto&&) { std::cout << "Unknown type" << std::endl; }
        );
    }
    
    // 测试3: 多参数匹配
    {
        std::variant<int, double> v1 = 42;
        std::variant<std::string, bool> v2 = true;
        
        // 注意：当前实现只支持单参数匹配
        // 如果需要多参数匹配，需要扩展实现
        Tools::static_match(v1,
            [](int i) { std::cout << "v1 is int: " << i << std::endl; },
            [](double d) { std::cout << "v1 is double: " << d << std::endl; }
        );
        
        Tools::static_match(v2,
            [](const std::string& s) { std::cout << "v2 is string: " << s << std::endl; },
            [](bool b) { std::cout << "v2 is bool: " << b << std::endl; }
        );
    }
    
    // 测试4: 编译时检查
    {
        std::variant<int, double> v = 3.14;
        
        // 这个会触发静态断言，因为没有匹配double的处理程序
        // Tools::static_match(v,
        //     [](int i) { std::cout << "Int: " << i << std::endl; }
        // );
        
        // 这个可以通过，因为有catch-all处理程序
        Tools::static_match(v,
            [](int i) { std::cout << "Int: " << i << std::endl; },
            [](auto&&) { std::cout << "Other type" << std::endl; }
        );
    }
    
    return 0;
}