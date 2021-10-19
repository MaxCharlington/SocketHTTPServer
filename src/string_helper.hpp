#pragma once

#include <type_traits>
#include <concepts>
#include <string>
#include <string_view>

template <class T>
constexpr std::string_view type_name()
{
    using namespace std;
    #ifdef __clang__
        std::string_view p = __PRETTY_FUNCTION__;
        return string_view(p.data() + 34, p.size() - 34 - 1);
    #elif defined(__GNUC__)
        std::string_view p = __PRETTY_FUNCTION__;
        #if __cplusplus < 201402
            return std::string_view(p.data() + 36, p.size() - 36 - 1);
        #else
            return std::string_view(p.data() + 49, p.find(';', 49) - 49);
        #endif
    #elif defined(_MSC_VER)
        std::string_view p = __FUNCSIG__;
        return std::string_view(p.data() + 84, p.size() - 84 - 7);
    #endif
}


template <typename T>
struct remove_all_const : std::remove_const<T>
{
};

template <typename T>
struct remove_all_const<T *>
{
    using type = remove_all_const<T>::type *;
};

template <typename T>
struct remove_all_const<T *const>
{
    using type = remove_all_const<T>::type *;
};

template <typename T>
using remove_all_const_t = remove_all_const<T>::type;


template <typename T>
concept Stringlike = []
{
    using type = remove_all_const_t<std::decay_t<T>>;
    return std::is_same_v<type, char *> ||
           std::is_same_v<type, std::string> ||
           std::is_same_v<type, std::string_view>;
}();

// Hacky way to concat stringlike objects. Update to reduce allocations
template <Stringlike Op1, Stringlike Op2>
std::string operator+(Op1 &&op1, Op2 &&op2)
{
    return std::string{std::forward<Op1>(op1)} + std::string{std::forward<Op2>(op2)};
}
