#pragma once

#include <utility>
#include <array>
#include <stdexcept>
#include <string_view>


enum struct HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE
};
constexpr auto HTTPMethodStrs = std::array<const char*, 4>{"GET", "POST", "PUT", "DELETE"};

constexpr HTTPMethod getHTTPMethod(std::string_view method) {
    for (size_t i = 0; i < HTTPMethodStrs.size(); i++) {
        if (HTTPMethodStrs[i] == method) {
            return static_cast<HTTPMethod>(i);
        }
    }
    throw std::runtime_error(std::string("Unknown HTTP method ") + method.data());  // method.data() should be properly terminated
}

constexpr std::string_view getHTTPMethodStr(HTTPMethod method) {
    return HTTPMethodStrs[std::to_underlying(method)];
}
