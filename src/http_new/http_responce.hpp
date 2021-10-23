#pragma once

#include <string>
#include <string_view>
#include <charconv>

#include "http_common.hpp"
#include "http_response_status.hpp"
#include "string_like.hpp"


class Response {
    std::string m_raw_response;

    std::string_view m_protocol;
    size_t m_status;
    std::string_view m_status_msg;
    HeadersRef m_headers;
    std::string_view m_content;

public:
    static const std::string_view DEFAULT_PROTOCOL;

    Response();
    Response(const Response&) = delete;
    Response(Response&&);
    Response& operator=(const Response&) = delete;
    Response& operator=(Response&&);

    Response(string_like auto&& protocol, uint16_t status);
    Response(string_like auto&& response_str);


    auto getProtocol() const -> std::string { return std::string{m_protocol}; }
    auto getProtocolRef() const -> std::string_view { return m_protocol; }
    void setProtocol(string_like auto&& protocol);

    auto getStatus() const -> uint16_t { return m_status; }
    auto setStatus(uint16_t status);

    auto getStatusMsg() const -> std::string { return std::string{m_status_msg}; }
    auto getStatusMsgRef() const -> std::string_view { return m_status_msg; }

    auto getHeaderValue(string_like auto&& key) const -> std::string;
    auto getHeaderValueRef(string_like auto&& key) const -> std::string_view;
    auto getHeaders() const -> Headers;
    auto getHeadersRef() const -> HeadersRef { return m_headers; }
    void addHeader(string_like auto&& key, std::string_view value);
    void addHeader(Header header);

    auto getContent() const -> std::string { return std::string{m_content}; }
    auto getContentRef() const -> std::string_view { return m_content; }
    void setContent(string_like auto&& content);

    auto toString() const -> std::string { return m_raw_response; }
    auto toStringRef() const -> std::string_view { return m_raw_response; }
};


const auto Response::DEFAULT_PROTOCOL = std::string_view{protocol_versions[1]};


using std::string_view;
using std::string;
using std::to_chars;
using std::to_string;
using std::from_chars;
using std::move;
#define forward_marco(var) std::forward<decltype(var)>(var)

[[nodiscard]] Response::Response()
    : m_raw_response{"HTTP/1.1 200 OK"},
    m_protocol{string_view{m_raw_response}.substr(0, protocol_versions[1].length() - 1)},
    m_status{200},
    m_status_msg{m_protocol.begin() + 2 + statusLength + 2, string_view{m_raw_response}.end() - 1}
{}

Response::Response(Response&& response)
    : m_raw_response{move(response.m_raw_response)},
    m_protocol{response.m_protocol},
    m_status{response.m_status},
    m_status_msg{response.m_status_msg}
{}

Response& Response::operator=(Response&& response) {
    m_raw_response = move(response.m_raw_response);
    m_protocol = response.m_protocol;
    m_status = response.m_status;
    m_status_msg = response.m_status_msg;
}

Response::Response(string_like auto&& protocol, uint16_t status)
    : m_raw_response{forward_marco(protocol)},
    m_protocol{m_raw_response.begin(), m_raw_response.end() - 1},
    m_status{[&]{
        assertIsValid(status);
        // Appending status and message to raw responce
        m_raw_response += ' ' += to_string(status) += ' ' += getHTTPStatusMessage(status);
        return status;
    }()},
    m_status_msg{m_protocol.end() + statusLength + 2, getHTTPStatusMessage(status).length()}
{}

// Response::Response(string_like auto&& response_str)
//     : m_raw_response{forward_marco(response_str)}


void Response::setProtocol(string_like auto&& protocol) {
    m_raw_response.replace(0, std::size(protocol), protocol);
    m_protocol = {m_raw_response.begin(), std::size(protocol)};
}

// auto Response::setStatus(uint16_t status) {
//     assertIsValid(status);
//     m_raw_response.replace(m_raw_response.find(to_string(m_status)), 3, to_string(status));
//     m_status = status;
//     // Update message
// }

auto Response::getHeaderValue(string_like auto&& key) const -> std::string {
    return std::string{getHeaderImpl(m_headers), forward_marco(key)};
}

auto Response::getHeaderValueRef(string_like auto&& key) const -> std::string_view {
    return getHeaderImpl(m_headers), forward_marco(key);
}

auto Response::getHeaders() const -> Headers {
    return toHeaders(m_headers);
}
// void Response::addHeader(string_like auto&& key, string_like auto&& value);
// void Response::addHeader(Header header);

// void Response::setContent(string_like auto&& content) {
//     m_raw_response.replace(m_raw_response.length() - m_content.length(), std::size(content), content);  // ?
// }
