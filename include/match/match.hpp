#pragma once
#include <iostream>
#include <functional>
#include <variant>
#include <vector>
#include <optional>
#include <type_traits>
#include <tuple>
#include <stdexcept>
namespace Tools {
// 修正的contains模板，两个模板参数，一个是类型列表，一个是待查找类型
template<typename Tuple, typename T>
struct contains;

template<typename T>
struct contains<std::tuple<>, T> : std::false_type {};

template<typename Head, typename... Tail, typename T>
struct contains<std::tuple<Head, Tail...>, T>
    : std::conditional_t<std::is_same_v<Head, T>, std::true_type, contains<std::tuple<Tail...>, T>> {};

// 元组去重辅助
template<typename... Ts>
struct unique_types;

template<>
struct unique_types<> {
    using type = std::tuple<>;
};

template<typename T, typename... Ts>
struct unique_types<T, Ts...> {
private:
    using rest_unique = typename unique_types<Ts...>::type;

public:
    using type = std::conditional_t<
        contains<rest_unique, T>::value,
        rest_unique,
        decltype(std::tuple_cat(std::declval<std::tuple<T>>(), std::declval<rest_unique>()))>;
};

template<typename... Ts>
using unique_types_t = typename unique_types<Ts...>::type;

template<typename Tuple>
struct tuple_to_variant;

template<typename... Ts>
struct tuple_to_variant<std::tuple<Ts...>> {
    using type = std::variant<Ts...>;
};


// --------- Match实现 ----------

template<typename T, typename RetTuple = std::tuple<>>
class Match {
    using Variant = typename tuple_to_variant<RetTuple>::type;

    using Predicate = std::function<bool(const T&)>;
    using ActionFunc = std::function<Variant(const T&)>;

    struct CaseType {
        Predicate pred;
        ActionFunc action;
    };

public:
    Match() = default;
    explicit Match(const T& val) : value(val) {}

    template<typename F>
    auto Case(const T& pattern, F&& func) const {

    }

    template<typename Pred, typename F>
    auto CaseWhen(Pred&& pred, F&& func) const {
        
    }

    template<typename F>
    auto Default(F&& func) const {
        
    }

    Variant Run() const {

    }

private:
    T value;
    std::vector<CaseType> cases;
    std::optional<ActionFunc> default_action;
};


}