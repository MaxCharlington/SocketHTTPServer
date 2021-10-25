#pragma once

#include <string_view>
#include <type_traits>


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


template <typename UnderlingType, typename Tag>
class StrongType
{
    UnderlingType m_value;

public:
    explicit StrongType(const UnderlingType& value) : m_value{value} {}

    UnderlingType get() {
        return m_value;
    }
};


template <typename T>
struct remove_all_const : std::remove_const<T> {};

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


template<typename T1, typename T2>
concept same_to = std::is_same_v<std::decay_t<T1>, std::decay_t<T2>>;
