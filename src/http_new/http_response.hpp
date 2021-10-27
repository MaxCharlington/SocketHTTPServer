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

    std::string getStartLine() final;

public:
    static std::string_view DEFAULT_PROTOCOL;

    Response();
    Response(const Response&) = default;
    Response(Response&&) = default;
    Response& operator=(const Response&) = default;
    Response& operator=(Response&&) = default;

    Response(string_like auto&& protocol, uint16_t status);
    Response(string_like auto&& response_str);


    [[nodiscard]] auto getProtocol() const -> std::string { return m_protocol; }
    [[nodiscard]] auto getProtocolView() const -> std::string_view { return m_protocol; }
    void setProtocol(string_like auto&& protocol);

    [[nodiscard]] auto getStatus() const -> HTTPStatus { return std::stoi(m_status); };
    auto setStatus(HTTPStatus status);

    [[nodiscard]] auto getStatusMsg() const -> std::string { return m_status_msg; }
    [[nodiscard]] auto getStatusMsgView() const -> std::string_view { return m_status_msg; }

};


std::string_view Response::DEFAULT_PROTOCOL = std::string_view{protocol_versions[1]};

[[nodiscard]] std::string Response::getStartLine() {
    return m_protocol + ' ' + m_status + ' ' + m_status_msg;
}


Response::Response()
    : Message{},
    m_protocol{protocol_versions[1]},
    m_status{"200"},
    m_status_msg{"OK"}
{
    init();
}

Response::Response(string_like auto&& protocol, uint16_t status)
    : Message{},
    m_protocol{std::forward<decltype(protocol)>(protocol)},
    m_status{std::to_string(status)},
    m_status_msg{getHTTPStatusMessage(status)}
{
    assertIsValid(status);
    init();
}

Response::Response(string_like auto&& response_str)
    : Message{std::forward<decltype(response_str)>(response_str)}
{
    auto lines = split(m_raw_message, newLine);
    auto start_line_items = split(std::move(lines[0]), " ");
    m_protocol = std::move(start_line_items[0]);
    m_status = std::move(start_line_items[1]);
    m_status_msg = std::move(start_line_items[2]);
    parseBody(std::move(lines));
    // No init as m_raw message is already initialized
}


void Response::setProtocol(string_like auto&& protocol) {
    if (isValid(protocol)) {
        m_protocol = std::forward<decltype(protocol)>(protocol);
        init();
    }
    else throw std::runtime_error("Unknown protocol: "s + std::forward<decltype(protocol)>(protocol));
}

auto Response::setStatus(uint16_t status) {
    assertIsValid(status);
    m_status = std::to_string(status);
    init();
}

} // namespace HTTP
