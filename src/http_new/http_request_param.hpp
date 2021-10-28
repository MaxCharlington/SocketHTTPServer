#pragma once

#include <string>
#include <string_view>
#include <unordered_map>


namespace HTTP {

template <typename T>
using URNParam = std::pair<T, T>;

template <typename T>
using URNParams = std::unordered_map<T, T>;

using ParamView = URNParam<std::string_view>;
using ParamsView = URNParams<std::string_view>;
using Param = URNParam<std::string>;
using Params = URNParams<std::string>;

} // namespace HTTP
