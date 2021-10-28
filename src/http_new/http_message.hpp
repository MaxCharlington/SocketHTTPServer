#pragma once

#include <vector>
#include <stdexcept>
#include <string>
#include <string_view>
#include <algorithm>

#include "http_header.hpp"
#include "helpers.hpp"
#include "string_like.hpp"
#include "string_helper.hpp"


namespace HTTP {

class Message {
protected:
    static constexpr auto newLine = "\r\n";

    Headers m_headers;
    std::string m_content;


    virtual std::string getStartLine() const = 0;

    Message() = default;
    Message(Headers&& headers, std::string&& content)
        : m_headers{std::move(headers)}, m_content{std::move(content)} {}
    virtual ~Message() = 0;

    void parseBody(std::vector<std::string_view>&& lines);


    [[nodiscard]] std::string getMessageBody() const;
    void addContent(string_like auto&& text);
    auto getHeaderValueRef(string_like auto&& key) const -> const std::string&;
    auto getHeaderValueRef(string_like auto&& key) -> std::string&;

public:
    template <typename Ret = std::string>
    auto getHeaderValue(string_like auto&& key) const -> Ret { return getHeaderValueRef(std::forward<decltype(key)>(key)); }
    auto getHeaders() const -> const Headers& { return m_headers; }
    void addHeader(string_like auto&& key, string_like auto&& value);
    void addHeader(same_as<Header> auto&& header);

    [[nodiscard]] auto getContent() const -> std::string { return m_content; }
    [[nodiscard]] auto getContentView() const -> std::string_view { return m_content; }
    void setContent(string_like auto&& content);

    [[nodiscard]] auto toString() const -> std::string { return getStartLine() + getMessageBody(); }
};

Message::~Message() {}


void Message::parseBody(std::vector<std::string_view>&& lines)
{
    if (lines.size() > 1) {
        size_t last_header_line_num = lines.size() - 1;
        for (size_t i = 1; i <= last_header_line_num; ++i) {
            auto header_parts = split(lines[i], ": ");
            addHeader(header_parts[0], header_parts[1]);
            if (header_parts[0] == "Content-Type")
                last_header_line_num -= 2;  // Last 2 lines are new line and content
        }
        m_content = lines[lines.size() - 1];
    }
}

[[nodiscard]] std::string Message::getMessageBody() const {
    std::string body;
    if (m_headers.size() > 0) {
        body += newLine;
        for (const auto& [key, value] : m_headers)
            body += key + ": " + value + newLine;
        if (m_content.length() > 0)
            (body += newLine) += m_content;
    }
    return body;
}

void Message::addContent(string_like auto&& text) {
    m_content = std::forward<decltype(text)>(text);
    m_headers.emplace_back("Content-Length", std::to_string(m_content.length()));
    m_headers.emplace_back("Content-Type", "text/plain");
}

[[nodiscard]] auto Message::getHeaderValueRef(string_like auto&& searched_key) const -> const std::string& {
    if (m_headers.size() > 0)
        for (const auto& [key, value] : m_headers)
            if (key == searched_key)
                return value;
    throw std::runtime_error{"There was no key in headers called" + std::string{searched_key}};
}

[[nodiscard]] auto Message::getHeaderValueRef(string_like auto&& searched_key) -> std::string& {
    if (m_headers.size() > 0)
        for (auto& [key, value] : m_headers)
            if (key == searched_key)
                return value;
    throw std::runtime_error{"There was no key in headers called" + std::string{searched_key}};
}

void Message::addHeader(string_like auto&& key, string_like auto&& value) {
    // TODO: Add checking for header validity
    m_headers.emplace_back(std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
}

void Message::addHeader(same_as<Header> auto&& header) {
    // TODO: Add checking for header validity
    m_headers.emplace_back(std::forward<decltype(header)>(header));
}

void Message::setContent(string_like auto&& content) {
    if (m_content == "")
        addContent(std::forward<decltype(content)>(content));
    else {
        m_content = std::forward<decltype(content)>(content);
        getHeaderValueRef("Content-Length") = m_content.length();
    }
}

} // namespace HTTP
