#pragma once

#include <string_view>
#include <string>
#include <utility>


namespace HTTP {

template <typename T>
using URNParam = std::pair<T, T>;

template <typename T>
using URNParams = std::vector<URNParam<T>>;

using ParamView = URNParam<std::string_view>;
using ParamsView = URNParams<std::string_view>;
using Param = URNParam<std::string>;
using Params = URNParams<std::string>;

} // namespace HTTP
