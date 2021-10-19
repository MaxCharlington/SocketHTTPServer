#pragma once

#include <array>
#include <vector>
#include <stdexcept>
#include <string_view>
#include <cstring>

#include "rest.hpp"
#include "http_header.hpp"
#include "cookie.hpp"
#include "../string_helpers.hpp"

/*
    Response and request objects are valid for the lifetime of content of recieved data buffer
    Add temporary storage for buffer and replace pointers with string_views pionting to copied
    buffer
*/

constexpr auto newLine = "\r\n";

struct Request {

    HTTPMethod method;
    char* uri;       // before '?'
    char* params;    // "a=1&b=2" things after  '?'
    char* protocol;  // "HTTP/<version>"

    using Header_t = Header<>;
    std::vector<Header_t> headers;
    std::string content;

    Request() = default;
    Request(char* req);
    Request(HTTPMethod method_, char* uri_, char* params_ = "", char* protocol_ = "HTTP/1.1")
        : method{method_}, uri{uri_}, params{params_}, protocol{protocol_}, headers{}, content{} {}

    auto getHeader(std::string_view key) -> std::string_view;
    void addHeader(std::string key, std::string value);
    void addContent(std::string content_);
    auto toString() const -> std::string;
};

Request::Request(char* req) {
    method = getHTTPMethod(strtok(req, " \t\r\n"));
    uri = strtok(nullptr, " \t");
    protocol = strtok(nullptr, " \t\r\n");

    if (params = strchr(uri, '?'); params)
        *params++ = '\0'; //split URI   // ?
    else
        params = uri - 1; //use an empty string

    const char *t;
    while (true)
    {
        const char* key = strtok(nullptr, "\r\n: \t");
        if (!key)
            break;
        const char* value = strtok(nullptr, "\r\n");
        while (*value && *value == ' ')
            value++;
        addHeader(key, value);
        t = value + 1 + strlen(value);
        if (t[1] == '\r' && t[2] == '\n') {  // ?
            t += 3;  // ?
            break;
        }
    }
    content = t;
}

std::string_view Request::getHeader(std::string_view key) {
    for (auto& header : headers) {
        if (header.key == key)
            return header.value;
    }
    throw std::runtime_error(std::string("There is no header in request with key: ") + key);
}

void Request::addHeader(std::string key, std::string value) {
    headers.emplace_back(std::move(key), std::move(value));
}

void Request::addContent(std::string content_) {
    content = std::move(content_);
    headers.emplace_back("Content-Length", std::to_string(content.length()));
    headers.emplace_back("Content-Type", "text/plain");
}

std::string Request::toString() const {
    auto get_params = [&]() -> std::string {
        if (strlen(params) > 0) {
            return std::string("?") + params;
        }
        return "";
    };
    auto req_str = getHTTPMethodStr(method) + ' ' + uri + get_params() + ' ' + protocol + newLine;
    for (const auto& header : headers) {
        req_str += std::string(header.key) + ": " + header.value + newLine;
    }
    req_str += newLine;
    req_str += content;
    return req_str;
}


struct Response {
    char* protocol;
    uint16_t status;
    char* status_message;

    using Header_t = Header<>;
    std::vector<Header_t> headers;

    std::string content;

    Response() = default;
    Response(char* protocol_, uint16_t status_, char* status_message_)
        : protocol{protocol_}, status{status_}, status_message{status_message_}, headers{}, content{} {}
    Response(char* res);

    auto getHeader(std::string_view key) -> std::string_view;
    void addHeader(std::string key, std::string value);
    void addContent(std::string content_);
    auto toString() const -> std::string;
};

Response::Response(char* res) {
    protocol = strtok(res, " \t");
    status = static_cast<uint16_t>(atoi(strtok(nullptr, " \t")));
    status_message = strtok(nullptr, "\t\r\n");

    const char *t;
    while (true)
    {
        const char* key = strtok(nullptr, "\r\n: \t");
        if (!key)
            break;
        const char* value = strtok(nullptr, "\r\n");
        while (*value && *value == ' ')
            value++;
        headers.emplace_back(key, value);
        t = value + 1 + strlen(value);
        if (t[1] == '\r' && t[2] == '\n') {
            t += 3;
            break;
        }
    }
    content = t;
}

std::string_view Response::getHeader(std::string_view key) {
    for (auto& header : headers) {
        if (header.key == key)
            return header.value;
    }
    throw std::runtime_error(std::string("There is no header in request with key: ") + key);;
}

void Response::addHeader(std::string key, std::string value) {
    headers.emplace_back(std::move(key), std::move(value));
}

void Response::addContent(std::string content_) {
    content = std::move(content_);
    headers.emplace_back("Content-Length", std::to_string(content.length()));
    headers.emplace_back("Content-Type", "text/plain");
}

std::string Response::toString() const {
    auto resp_str = std::string(protocol) + ' ' + std::to_string(status) + ' ' + status_message + newLine;
    for (const auto& header : headers) {
        resp_str += header.key + ": " + header.value + newLine;
    }
    resp_str += newLine;
    resp_str += content;
    return resp_str;
}
