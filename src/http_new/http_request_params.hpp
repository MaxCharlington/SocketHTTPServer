#pragma once

#include <string_view>
#include <string>
#include <utility>


template <typename T>
using URIParam = std::pair<T, T>;

template <typename T>
using URIParams = std::vector<URIParam<T>>;

using ParamRef = URIParam<std::string_view>;
using ParamsRef = URIParams<std::string_view>;
using Param = URIParam<std::string>;
using Params = URIParams<std::string>;
