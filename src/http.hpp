#pragma once

#include <utility>
#include <bit>
#include <array>
#include <vector>
#include <stdexcept>
#include <string_view>
#include <cstring>

enum struct HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE
};
constexpr auto HTTPMethodStrs = std::array<const char*, 4>{"GET", "POST", "PUT", "DELETE"};

HTTPMethod getHTTPMethod(std::string_view method) {
    for (size_t i = 0; i < HTTPMethodStrs.size(); i++) {
        if (HTTPMethodStrs[i] == method) {
            return static_cast<HTTPMethod>(i);
        }
    }
    throw std::runtime_error(std::string("Unknown HTTP method ") + method.data());  // method.data() should be properly terminated
}

const char* getHTTPMethodStr(HTTPMethod method) {
    return HTTPMethodStrs[std::to_underlying(method)];
}

template <typename KeyT = const char*, typename ValueT = const char*>
struct Header
{
    using key_type = KeyT;
    using value_type = ValueT;

    KeyT key;
    ValueT value;

    Header() = default;
    Header(key_type key_, value_type value_) : key{key_}, value{value_} {}
};


struct Request {
    using Header_t = Header<>;

    HTTPMethod method;    // GET or POST
    char* uri;       // "/index.html" things before '?'
    char* params;    // "a=1&b=2"     things after  '?'
    char* protocol;  // "HTTP/1.1"
    std::vector<Header_t> headers;
    const char *payload; // for POST
    int payload_size;

    Request() = default;
    Request(char* req);

    Request::Header_t::value_type get_header(std::string_view key);
};

Request::Request(char* req) {
    method = getHTTPMethod(strtok(req, " \t\r\n"));
    uri = strtok(nullptr, " \t");
    protocol = strtok(nullptr, " \t\r\n");

    fprintf(stderr, "\x1b[32m [%s] %s\x1b[0m\n", getHTTPMethodStr(method), uri);

    if (params = strchr(uri, '?'); params)
        *params++ = '\0'; //split URI
    else
        params = uri - 1; //use an empty string

    char *t;
    while (true)
    {
        char* key = strtok(nullptr, "\r\n: \t");
        if (!key)
            break;
        char* value = strtok(nullptr, "\r\n");
        while (*value && *value == ' ')
            value++;
        headers.emplace_back(key, value);
        fprintf(stderr, "\t[H] %s: %s\n", key, value);
        t = value + 1 + strlen(value);
        if (t[1] == '\r' && t[2] == '\n')
            break;
    }
    puts("");
    t++;  // now the *t shall be the beginning of user payload
    const char* t2 = get_header("Content-Length"); // and the related header if there is
    payload = t;
    //payload_size = t2 ? atol(t2) : (rcvd - (t - buf));
    payload_size = t2 ? atol(t2) : 0;  // TMP
}

Request::Header_t::value_type Request::get_header(std::string_view key) {
    for (auto header : headers) {
        if (header.key == key)
            return header.value;
    }
    return Header_t::value_type{};
}

