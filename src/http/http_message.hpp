#pragma once

#include <vector>
#include <stdexcept>
#include <string>
#include <string_view>
#include <algorithm>
#include <span>

#include "http_header.hpp"
#include "helpers.hpp"
#include "string_like.hpp"
#include "string_helper.hpp"


namespace http {


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


void Message::parseBody(std::vector<std::string_view>&& lines_)
{
    std::span lines{lines_.data() + 1, lines_.size() - 1};  // without start line
    bool handled_headers = false;
    for (const auto &line : lines)
    {
        if (!handled_headers) {
            if (line == "") {
                handled_headers = true;
                continue;
            }
            auto header_parts = split(line, ": ");
            addHeader(header_parts[0], header_parts[1]);
        }
        else {
            m_content += line;
        }
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
    m_headers.insert_or_assign("Content-Length", std::to_string(m_content.length()));
    m_headers.insert_or_assign("Content-Type", "text/plain");
}

[[nodiscard]] auto Message::getHeaderValueRef(string_like auto&& searched_key) const -> const std::string& {
    if (auto search = m_headers.find(searched_key); search != m_headers.end())
        return search->second;
    throw std::runtime_error{"There was no key in headers called " + std::string{searched_key}};
}

[[nodiscard]] auto Message::getHeaderValueRef(string_like auto&& searched_key) -> std::string& {
    if (auto search = m_headers.find(searched_key); search != m_headers.end())
        return search->second;
    throw std::runtime_error{"There was no key in headers called " + std::string{searched_key}};
}

void Message::addHeader(string_like auto&& key, string_like auto&& value) {
    // TODO: Add checking for header validity
    m_headers.insert_or_assign(std::string{std::forward<decltype(key)>(key)}, std::string{std::forward<decltype(value)>(value)});
}

void Message::addHeader(same_as<Header> auto&& header) {
    // TODO: Add checking for header validity
    auto&&[key, value] = header;
    m_headers.insert_or_assign(std::string{std::forward<decltype(key)>(key)}, std::string{std::forward<decltype(value)>(value)});
}

void Message::setContent(string_like auto&& content) {
    if (m_content == "")
        addContent(std::forward<decltype(content)>(content));
    else {
        m_content = std::forward<decltype(content)>(content);
        getHeaderValueRef("Content-Length") = m_content.length();
    }
}


} // namespace http
