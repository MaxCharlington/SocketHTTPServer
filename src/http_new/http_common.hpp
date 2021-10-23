#pragma once

#include <vector>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <algorithm>

using namespace std::literals;  // To hide


template <typename T>
using HTTPHeader = std::pair<T, T>;

template <typename T>
using HTTPHeaders = std::vector<HTTPHeader<T>>;

using HeaderRef = HTTPHeader<std::string_view>;
using HeadersRef = HTTPHeaders<std::string_view>;
using Header = HTTPHeader<std::string>;
using Headers = HTTPHeaders<std::string>;

Headers parceHeaders(std::string_view headers_str);

[[nodiscard]] std::string_view getHeaderImpl(const HeadersRef& headers, Stringlike auto&& searched_key) {
    for (auto&& [key, value] : headers) {
        if (key == searched_key)
            return value;
    }
    throw std::runtime_error{"There was no key in headers called" + std::string{searched_key}};
}

[[nodiscard]] Headers toHeaders(const HeadersRef& headersRef) {
    Headers headers;
    headers.reserve(headersRef.size());
    for (auto [key, value] : headersRef)
        headers.emplace_back(std::string{key}, std::string{value});
    return headers;
}

const std::array protocol_versions = {"HTTP/1.0"sv, "HTTP/1.1"sv, "HTTP/2"sv, "HTTP/3"sv};

[[nodiscard]] bool isValid(Stringlike auto&& protocol) {
    return std::ranges::any_of(
        protocol_versions,
        [protocol](auto&& prot){ return prot == protocol; }
    );
}
