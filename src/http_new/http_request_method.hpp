#pragma once

#include <utility>
#include <string_view>
#include <cassert>
#include <stdexcept>

#include "string_helper.hpp"

using namespace std::string_literals;


enum class HTTPMethod {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

constexpr std::string_view toString(HTTPMethod method) noexcept {
    switch (method)
    {
        using enum HTTPMethod;
        [[likely]] case GET: return "GET";
        case HEAD: return "HEAD";
        [[likely]] case POST: return "POST";
        [[likely]] case PUT: return "PUT";
        [[likely]] case DELETE: return "DELETE";
        case CONNECT: return "CONNECT";
        case OPTIONS: return "OPTIONS";
        case TRACE: return "TRACE";
        [[unlikely]] case PATCH: return "PATCH";
        [[unlikely]] default: throw std::runtime_error("Invalid HTTP method" + std::to_string(std::to_underlying(method)));
    }
}

template <typename Ret, Stringlike T>
constexpr HTTPMethod fromString(T&& method_str) noexcept {
    using enum HTTPMethod;
    [[likely]] if (method_str == "GET") return GET;
    if (method_str == "HEAD") return HEAD;
    [[likely]] if (method_str == "POST") return POST;
    [[likely]] if (method_str == "PUT") return PUT;
    [[likely]] if (method_str == "DELETE") return DELETE;
    if (method_str == "CONNECT") return CONNECT;
    if (method_str == "OPTIONS") return OPTIONS;
    if (method_str == "TRACE") return TRACE;
    [[unlikely]] if (method_str == "PATCH") return PATCH;
    throw std::runtime_error("Invalid HTTP method" + std::string(method_str));
}

using Method = HTTPMethod;
