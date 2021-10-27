#pragma once

#include <type_traits>

#include "helpers.hpp"


template <typename T>
concept string_like = []
{
    using type = remove_all_const_t<std::decay_t<T>>;
    return std::is_same_v<type, char *> ||
           std::is_same_v<type, std::string> ||
           std::is_same_v<type, std::string_view>;
}();
