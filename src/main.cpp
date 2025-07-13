// --- Test Example ---
#include <variant>
#include <string>
#include <iostream>

#include "../include/match/static_match.hpp"
#include "../include/match/match.hpp"
void test_static_match(){

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
    
}

void test_match(){
    int x = 42;

    auto matcher = Tools::Match<int>(x)
        .case_(1, [](int v) { return std::string("one: ") + std::to_string(v); })
        .case_when([](int v) { return v > 10 && v < 50; }, [](int v) { return v * 2.5; })
        .case_(42, [](int) { return std::vector<int>{4, 2, 1}; })
        .default_([](int v) { return "default: " + std::to_string(v); });

    auto result = matcher.run();

    std::visit([](auto&& val) {
        using V = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<V, std::vector<int>>) {
            std::cout << "Vector: ";
            for (auto i : val) std::cout << i << ' ';
        }
        else {
            std::cout << val;
        }
        std::cout << '\n';
    }, result);
}


int main() {
    test_match();
    return 0;
}