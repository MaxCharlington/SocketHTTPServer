#pragma once

#include <vector>
#include <string>
#include <string_view>


namespace HTTP {

template <typename T>
using HTTPHeader = std::pair<T, T>;

template <typename T>
using HTTPHeaders = std::vector<HTTPHeader<T>>;

using HeaderView = HTTPHeader<std::string_view>;
using HeadersView = HTTPHeaders<std::string_view>;
using Header = HTTPHeader<std::string>;
using Headers = HTTPHeaders<std::string>;

} // namespace HTTP
