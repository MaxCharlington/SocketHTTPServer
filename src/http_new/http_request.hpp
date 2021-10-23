#pragma once

#include <string>
#include <string_view>

#include "http_common.hpp"
#include "http_request_params.hpp"
#include "http_request_method.hpp"
#include "string_like.hpp"


class Request {
    std::string m_raw_request;

    Method m_method;
    std::string_view m_uri;       // before '?'
    ParamsRef m_params;           // "a=1&b=2" things after  '?'
    std::string_view m_protocol;  // "HTTP/<version>"
    HeadersRef m_headers;
    std::string_view m_content;

public:
    Request();
    Request(const Request&) = delete;
    Request(Request&&);
    Request& operator=(const Request&) = delete;
    Request& operator=(Request&&);

    Request(Method method, string_like auto&& uri, Params params, string_like auto&& protocol, Headers headers, string_like auto&& content);
    Request(string_like auto&& request_str);


    auto getMethod() const -> Method { return m_method; }
    void setMethod(Method method);

    auto getUri() -> std::string_view { return m_uri; }
    void setUri(string_like auto&& uri);

    auto getParamValue(string_like auto&& key) const -> std::string;
    auto getParamValueRef(string_like auto&& key) const -> std::string_view;
    auto getParams() const -> Params;
    auto getParamsRef() const -> ParamsRef;
    void addParam(string_like auto&& key, string_like auto&& value);
    void addParam(Param param);

    auto getProtocol() const -> std::string { return std::string{m_protocol}; }
    auto getProtocolRef() const -> std::string_view { return m_protocol; }
    void setProtocol(string_like auto&& protocol);

    auto getHeaderValue(string_like auto&& key) const -> std::string;
    auto getHeaderValueRef(string_like auto&& key) const -> std::string_view;
    auto getHeaders() const -> Headers;
    auto getHeadersRef() const -> HeadersRef { return m_headers; }
    void addHeader(string_like auto&& key, std::string_view value);
    void addHeader(Header header);

    auto getContent() const -> std::string { return std::string{m_content}; }
    auto getContentRef() const -> std::string_view { return m_content; }
    void setContent(string_like auto&& content);

    auto toString() const -> std::string { return m_raw_request; }
    auto toStringRef() const -> std::string_view { return m_raw_request; }
};
