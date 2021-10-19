#pragma once

#include <array>
#include <vector>
#include <stdexcept>
#include <string_view>
#include <cstring>

#include "rest.hpp"
#include "http_header.hpp"
#include "cookie.hpp"
#include "../string_helper.hpp"

/*
    Response and request objects are valid for the lifetime of content of recieved data buffer
    Add temporary storage for buffer and replace pointers with string_views pionting to copied
    buffer
*/

constexpr auto newLine = "\r\n";


// Unified helpers
template <typename Message>
void addContentImpl(Message& mes, std::string text) {
    if (text != "") {
        mes.content = std::move(text);
        mes.headers.emplace_back("Content-Length", std::to_string(mes.content.length()));
        mes.headers.emplace_back("Content-Type", "text/plain");
    }
}

template <typename Message>
std::string getMessageBodyImpl(Message& mes) {
    std::string body;
    if (mes.headers.size() > 0) {
        body += newLine;
        for (const auto& header : mes.headers) {
            body += header.key + ": " + header.value + newLine;
        }
        if (mes.content.length() > 0) {
            body += newLine;  // To remove
            body += mes.content;
        }
    }
    return body;
}

const char* getParamsImpl(const char* uri) {
    if (char* params_ = strchr(const_cast<char*>(uri), '?'); params_) {
        *params_++ = '\0'; //split URI   // ?
        return params_;
    }
    return uri - 1;  //use an empty string
}

template <typename Message>
const char* parceHeadersAndGetContent(Message& mes) {
    const char *t = "";
    while (true)
    {
        const char* key = strtok(nullptr, "\r\n: \t");
        if (!key)
            break;
        const char* value = strtok(nullptr, "\r\n");
        while (*value && *value == ' ')
            value++;

        mes.addHeader(key, value);

        t = value + 1 + strlen(value);
        if (t[1] == '\r' && t[2] == '\n') {  // ?
            t += 3;  // ?
            break;
        }
    }
    return t;
}


struct Request {
    HTTPMethod method;
    const char* uri;       // before '?'
    const char* params;    // "a=1&b=2" things after  '?'
    const char* protocol;  // "HTTP/<version>"
    Headers headers;
    std::string content;

    Request() = default;
    Request(char* req);
    Request(HTTPMethod method_, char* uri_, char* params_ = "", char* protocol_ = "HTTP/1.1")
        : method{method_}, uri{uri_}, params{params_}, protocol{protocol_} {}

    auto getHeader(std::string_view key) -> std::string_view;
    void addHeader(std::string key, std::string value);
    void addHeader(Header_t header);
    auto getParam(std::string_view key) const -> std::string_view;
    void addContent(std::string content_);
    auto toString() const -> std::string;
};

Request::Request(char* req) {
    method = getHTTPMethod(strtok(req, " \t\r\n"));
    uri = strtok(nullptr, " \t");
    protocol = strtok(nullptr, " \t\r\n");
    params = getParamsImpl(uri);
    content = parceHeadersAndGetContent(*this);
}

std::string_view Request::getHeader(std::string_view key) {
    return getHeaderImpl(headers, key);
}

void Request::addHeader(std::string key, std::string value) {
    headers.emplace_back(std::move(key), std::move(value));
}

void Request::addHeader(Header_t header) {
    if (header.key != "")
        headers.push_back(std::move(header));
}

std::string_view Request::getParam(std::string_view key) const {
    const char value_delim = '=';
    const char pair_delim = '&';

    size_t start = 0;
    size_t pos = 0;
    std::string_view cur_key;

    size_t param_length = strlen(params);
    for (; pos < param_length; pos++)
    {
        if (params[pos] == value_delim) {
            cur_key = std::string_view{params + start, pos - start};
            start = pos + 1;
        }
        if (params[pos] == pair_delim) {
            if (key == cur_key) return {params + start, pos - start};
        }
    }
    if (key == cur_key) return {params + start, pos - start};
    return "";
}

void Request::addContent(std::string content_) {
    addContentImpl(*this, content_);
}

std::string Request::toString() const {
    auto get_params = [&]() -> std::string {
        if (strlen(params) > 0) {
            return std::string("?") + params;
        }
        return "";
    };
    auto req_str = getHTTPMethodStr(method) + ' ' + uri + get_params() + ' ' + protocol;
    req_str += getMessageBodyImpl(*this);
    return req_str;
}


struct Response {
    const char* protocol;
    uint16_t status;
    const char* status_message;
    Headers headers;
    std::string content;

    Response() = default;
    Response(char* protocol_, uint16_t status_, char* status_message_)
        : protocol{protocol_}, status{status_}, status_message{status_message_} {}
    Response(char* res);

    auto getHeader(std::string_view key) -> std::string_view;
    void addHeader(std::string key, std::string value);
    void addHeader(Header_t header);
    void addContent(std::string content_);
    auto toString() const -> std::string;
};

Response::Response(char* res) {
    protocol = strtok(res, " \t");
    status = static_cast<uint16_t>(atoi(strtok(nullptr, " \t")));
    status_message = strtok(nullptr, "\t\r\n");

    content = parceHeadersAndGetContent(*this);
}

std::string_view Response::getHeader(std::string_view key) {
    return getHeaderImpl(headers, key);
}

void Response::addHeader(std::string key, std::string value) {
    headers.emplace_back(std::move(key), std::move(value));
}

void Response::addHeader(Header_t header) {
    headers.push_back(std::move(header));
}

void Response::addContent(std::string content_) {
    addContentImpl(*this, content_);
}

std::string Response::toString() const {
    auto resp_str = std::string(protocol) + ' ' + std::to_string(status) + ' ' + status_message;
    resp_str += getMessageBodyImpl(*this);
    return resp_str;
}
