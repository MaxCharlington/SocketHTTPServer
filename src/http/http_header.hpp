#pragma once

#include <string>
#include <string_view>
#include <unordered_map>


namespace http {


template <typename T>
using HttpHeader = std::pair<T, T>;

template <typename T>
using HttpHeaders = std::unordered_map<T, T>;

using HeaderView = HttpHeader<std::string_view>;
using HeadersView = HttpHeaders<std::string_view>;
using Header = HttpHeader<std::string>;
using Headers = HttpHeaders<std::string>;


} // namespace http
