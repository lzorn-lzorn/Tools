#pragma once

#include <type_traits>
#include <utility>
#include <variant>
namespace Tools {

// note: 继承多个函数对象(通常是 lambda), 并将其 operator 聚合, 即 lambda overloading
/* lambda overloading
 * auto f = overloaded{
 *   [](int i)   { std::cout << "int: " << i << "\n"; },
 *   [](float f) { std::cout << "float: " << f << "\n"; },
 *   [](auto x)  { std::cout << "unknown: " << x << "\n"; }
 * };
 * 等价于
 * struct F {
 *     void operator()(int i) { ... }
 *     void operator()(float f) { ... }
 *     template<typename T>
 *     void operator()(T x) { ... }
 * };
 */
template<typename... Fs>
struct overloaded : Fs... { 
    using Fs::operator()...; 
};


// note: 类模板参数推导指引 (CTAD)
// 写 overloaded{...} 时自动推导类型，而不需要写 overloaded<...>{...}
template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace detail {
    // 遍历 Variant 中的所有类型, 检测 F 是否对于每一个类型都可以调用, F(x) 能不能编译过
    // note: std::index_sequence<Is...> 用于生成整数序列 [1,2,3,... n]
    // note: std::variant_alternative_t<Is, Variant>> 获取第 I 个类型
    // note: std::is_invocable_v<F, T> 判读 F(t) 是否合法
    template <typename F, typename Variant, std::size_t... Is>
    constexpr bool is_fallback_for_variant_impl(std::index_sequence<Is...>) {
        return (std::is_invocable_v<F, std::variant_alternative_t<Is, Variant>> && ...);
    }

    // 把 Variant 中的所有类型对应的下标生成出来
    // note: variant_size_v 静态获取 variant 的size
    template <typename F, typename Variant>
    constexpr bool is_fallback_for_variant =
        is_fallback_for_variant_impl<F, Variant>(std::make_index_sequence<std::variant_size_v<Variant>>{});

    // 递归判断所有 Visitors(Fs, ...) 是否至少有一个可以对所有 Variant 类型调用
     template <typename Variant, typename... Fs>
    struct has_fallback_for_variant;

    template <typename Variant>
    struct has_fallback_for_variant<Variant> : std::false_type {};

    template <typename Variant, typename F, typename... Rest>
    struct has_fallback_for_variant<Variant, F, Rest...> {
        static constexpr bool value =
            is_fallback_for_variant<F, Variant> 
            || has_fallback_for_variant<Variant, Rest...>::value;
    };

    template<typename Ty>
    constexpr bool always_false = false;

    template<typename Variant, typename Visitor>
    concept matchable = requires(Visitor vis, Variant var){
        { std::visit(vis, var)};
    };

}
/* usage
std::variant<int, double, std::string> v = 3.14;

    match::static_match_ext(v,
        [](int i)       { std::cout << "int: " << i << "\n"; },
        [](double d)    { std::cout << "double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "string: " << s << "\n"; },
        [](auto&& other) { std::cout << "fallback for unknown: " << other << "\n"; }
    );
*/
template<typename Variant, typename... Visitors>
requires detail::matchable<Variant, overloaded<Visitors...>>
auto static_match_ext(const Variant& value, Visitors&&... visitors) {
    return std::visit(overloaded{std::forward<Visitors>(visitors)...}, value);
}

template<typename Variant, typename... Visitors>
requires (!detail::matchable<Variant, overloaded<Visitors...>>)
auto static_match_ext(const Variant&) {
    static_assert(detail::has_fallback_for_variant<Variant, Visitors...>::value,
        "No matching visitor found for all types in the variant. Consider adding a catch-all lambda: [](auto&&) {...}");
    return; // unreachable
}
}