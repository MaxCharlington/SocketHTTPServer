#pragma once

#include <string>
#include <string_view>
#include <optional>

#include "http_message.hpp"
#include "http_response_status.hpp"
#include "http_protocol.hpp"
#include "string_like.hpp"
#include "string_helper.hpp"
#include "helpers.hpp"


namespace HTTP {

class Response : public Message {
    std::string m_protocol;
    std::string m_status;
    std::string m_status_msg;

    std::string getStartLine() const final;

public:
    static std::string_view DEFAULT_PROTOCOL;

    Response()
        : m_protocol{protocol_versions[1]}, m_status{"200"}, m_status_msg{"OK"} {}
    Response(const Response&) = default;
    Response(Response&&) = default;
    Response& operator=(const Response&) = default;
    Response& operator=(Response&&) = default;

    Response(string_like auto&& protocol, uint16_t status);
    Response(string_like auto&& response_str);


    template <typename Ret = std::string>
    [[nodiscard]] auto getProtocol() const -> Ret { return m_protocol; }

    void setProtocol(string_like auto&& protocol);


    [[nodiscard]] auto getStatus() const -> HTTPStatus { return std::stoi(m_status); };

    auto setStatus(HTTPStatus status);


    template <typename Ret = std::string>
    [[nodiscard]] auto getStatusMsg() const -> Ret { return m_status_msg; }
};

std::string_view Response::DEFAULT_PROTOCOL = std::string_view{protocol_versions[1]};


[[nodiscard]] std::string Response::getStartLine() const {
    return m_protocol + ' ' + m_status + ' ' + m_status_msg;
}

Response::Response(string_like auto&& protocol, uint16_t status)
    : m_protocol{std::forward<decltype(protocol)>(protocol)},
    m_status{std::to_string(status)},
    m_status_msg{getHTTPStatusMessage(status)}
{
    assertIsValid(status);
}

Response::Response(string_like auto&& response_str)
{
    auto lines = split(std::forward<decltype(response_str)>(response_str), newLine);
    auto start_line_items = split(std::move(lines[0]), " ");
    m_protocol = std::move(start_line_items[0]);
    m_status = std::move(start_line_items[1]);
    m_status_msg = std::move(start_line_items[2]);
    parseBody(std::move(lines));
}


void Response::setProtocol(string_like auto&& protocol) {
    if (isValid(protocol)) {
        m_protocol = std::forward<decltype(protocol)>(protocol);
    }
    else throw std::runtime_error("Unknown protocol: "s + std::forward<decltype(protocol)>(protocol));
}

auto Response::setStatus(uint16_t status) {
    assertIsValid(status);
    m_status = std::to_string(status);
}

} // namespace HTTP
