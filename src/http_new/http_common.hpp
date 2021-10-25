#pragma once

#include <vector>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <algorithm>
#include <optional>
#include <functional>

#include "helpers.hpp"
#include "string_like.hpp"
#include "string_helper.hpp"

using namespace std::literals;  // To hide


template <typename T>
using HTTPHeader = std::pair<T, T>;

template <typename T>
using HTTPHeaders = std::vector<HTTPHeader<T>>;

using HeaderView = HTTPHeader<std::string_view>;
using HeadersView = HTTPHeaders<std::string_view>;
using Header = HTTPHeader<std::string>;
using Headers = HTTPHeaders<std::string>;

Headers parceHeaders(std::string_view headers_str);

[[nodiscard]] Headers toHeaders(const HeadersView& headersView) {
    Headers headers;
    headers.reserve(headersView.size());
    for (auto [key, value] : headersView)
        headers.emplace_back(std::string{key}, std::string{value});
    return headers;
}

[[nodiscard]] HeadersView toHeadersView(const Headers& headers) {
    HeadersView headersView;
    headersView.reserve(headers.size());
    for (const auto& [key, value] : headers)
        headersView.emplace_back(key, value);
    return headersView;
}


const std::array protocol_versions = {"HTTP/1.0"sv, "HTTP/1.1"sv, "HTTP/2"sv, "HTTP/3"sv};

[[nodiscard]] bool isValid(string_like auto&& protocol) {
    return std::ranges::any_of(
        protocol_versions,
        [protocol](auto&& prot){ return prot == protocol; }
    );
}


class Message {
protected:
    static constexpr auto newLine = "\r\n";

    std::string m_raw_message;
    std::optional<Headers> m_headers;
    std::string m_content;


    virtual std::string getStartLine() = 0;

    Message()
        : m_raw_message{"HTTP/1.1 200 OK"} {}
    Message(string_like auto&& raw_message)
        : m_raw_message{std::forward<decltype(raw_message)>(raw_message)} {}
    Message(std::optional<Headers>&& headers, std::string&& content)
        : m_headers{std::move(headers)}, m_content{std::move(content)} {}


    void parseBody(std::vector<std::string_view>&& lines);
    virtual ~Message() = 0;

    void init();

    [[nodiscard]] std::string getMessageBody() const;
    [[nodiscard]] std::string& getHeaderVal(string_like auto&& searched_key);
    void addContent(string_like auto&& text);
    auto getHeaderValueRef(string_like auto&& key) const -> const std::string&;
    auto getHeaderValueRef(string_like auto&& key) -> std::string&;

public:
    auto getHeaderValue(string_like auto&& key) const -> std::string;
    auto getHeaderValueView(string_like auto&& key) const -> std::string_view;
    auto getHeaders() const -> Headers;
    auto getHeadersView() const -> HeadersView;
    void addHeader(string_like auto&& key, string_like auto&& value);
    void addHeader(Header&& header);

    auto getContent() const -> std::string { return m_content; }
    auto getContentView() const -> std::string_view { return m_content; }
    void setContent(string_like auto&& content);

    auto toString() const -> std::string { return m_raw_message; }
    auto toStringView() const -> std::string_view { return m_raw_message; }
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

void Message::init() {
    m_raw_message = getStartLine() + getMessageBody();
}

[[nodiscard]] std::string Message::getMessageBody() const {
    std::string body;
    if (m_headers.has_value() && m_headers.value().size() > 0) {
        body += newLine;
        for (const auto& [key, value] : m_headers.value())
            body += key + ": " + value + newLine;
        if (m_content.length() > 0)
            (body += newLine) += m_content;
    }
    return body;
}

void Message::addContent(string_like auto&& text) {
    m_content = std::forward<decltype(text)>(text);
    std::reference_wrapper<Headers> headers;
    if (!m_headers.has_value())
        headers = m_headers.emplace();
    else
        headers = m_headers.value();
    headers.get().emplace_back("Content-Length", std::to_string(m_content.length()));
    headers.get().emplace_back("Content-Type", "text/plain");
    init();
}

[[nodiscard]] auto Message::getHeaderValueRef(string_like auto&& searched_key) const -> const std::string& {
    if (m_headers.has_value())
        for (const auto& [key, value] : m_headers.value())
            if (key == searched_key)
                return value;
    throw std::runtime_error{"There was no key in headers called" + std::string{searched_key}};
}

[[nodiscard]] auto Message::getHeaderValueRef(string_like auto&& searched_key) -> std::string& {
    if (m_headers.has_value())
        for (auto& [key, value] : m_headers.value())
            if (key == searched_key)
                return value;
    throw std::runtime_error{"There was no key in headers called" + std::string{searched_key}};
}

auto Message::getHeaderValue(string_like auto&& key) const -> std::string {
    return getHeaderValueRef(std::forward<decltype(key)>(key));
}

auto Message::getHeaderValueView(string_like auto&& key) const -> std::string_view {
    return getHeaderValueRef(std::forward<decltype(key)>(key));
}

auto Message::getHeaders() const -> Headers {
    return m_headers.has_value() ? m_headers.value() : Headers{};
}

auto Message::getHeadersView() const -> HeadersView {
    return toHeadersView(m_headers.has_value() ? m_headers.value() : Headers{});
}

void Message::addHeader(string_like auto&& key, string_like auto&& value) {
    // TODO: Add checking for header validity
    if (!m_headers.has_value()) {
        m_headers.emplace().emplace_back(std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
    }
    else {
        m_headers.value().emplace_back(std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
    }
    init();
}

void Message::addHeader(Header&& header) {
    // TODO: Add checking for header validity
    if (!m_headers.has_value()) {
        m_headers.emplace().emplace_back(std::move(header));
    }
    else {
        m_headers.value().emplace_back(std::move(header));
    }
    init();
}

void Message::setContent(string_like auto&& content) {
    if (m_content == "")
        addContent(std::forward<decltype(content)>(content));
    else {
        m_content = std::forward<decltype(content)>(content);
        getHeaderValueRef("Content-Length") = m_content.length();
    }
    init();
}
