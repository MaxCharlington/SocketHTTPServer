#pragma once

#include <type_traits>
#include <concepts>
#include <string>
#include <string_view>


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
           std::is_same_v<type, char> ||
           std::is_same_v<type, std::string> ||
           std::is_same_v<type, std::string_view>;
}();

// Hacky way to concat stringlike objects. Update to reduce allocations
template <Stringlike Op1, Stringlike Op2>
std::string operator+(Op1 &&op1, Op2 &&op2)
{
    return std::string{std::forward<Op1>(op1)} + std::string{std::forward<Op2>(op2)};
}
