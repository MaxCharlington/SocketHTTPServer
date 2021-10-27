#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <utility>

#include "http_message.hpp"
#include "http_request_param.hpp"
#include "http_request_method.hpp"
#include "string_like.hpp"
#include "helpers.hpp"


namespace HTTP {

class Request : public Message {
    Method m_method;
    std::string m_uri;       // before '?'
    std::optional<Params> m_params;         // "a=1&b=2" things after  '?'
    std::string m_protocol;  // "HTTP/<version>"

    std::string getStartLine() final;
    auto getParamValueRef(string_like auto&& key) const -> const std::string&;
    auto getParamValueRef(string_like auto&& key) -> std::string&;

public:
    Request(const Request&) = default;
    Request(Request&&) = default;
    Request& operator=(const Request&) = default;
    Request& operator=(Request&&) = default;

    Request(Method method, string_like auto&& uri, string_like auto&& protocol);
    Request(Method method, string_like auto&& uri, Params params, string_like auto&& protocol);
    Request(string_like auto&& request_str);


    [[nodiscard]] auto getMethod() const -> Method { return m_method; }
    void setMethod(Method method);

    [[nodiscard]] auto getUri() -> std::string_view { return m_uri; }
    void setUri(string_like auto&& uri);

    auto getParamValue(string_like auto&& key) const -> std::string;
    auto getParamValueView(string_like auto&& key) const -> std::string_view;
    auto getParams() const -> Params;
    auto getParamsView() const -> ParamsView;
    void addParam(string_like auto&& key, string_like auto&& value);
    void addParam(same_as<Param> auto&& param);

    [[nodiscard]] auto getProtocol() const -> std::string { return std::string{m_protocol}; }
    auto getProtocolView() const -> std::string_view { return m_protocol; }
    void setProtocol(string_like auto&& protocol);
};


[[nodiscard]] std::string Request::getStartLine() {
    std::string path_params = m_uri;
    if (m_params.has_value()) {
        path_params += "?";
        for (auto &&[key, value] : m_params.value())
        {
            path_params += key + "=" + value + "&";
        }
        path_params.pop_back();
    }
    return std::to_string(std::to_underlying(m_method)) + ' ' + path_params + ' ' + m_protocol;
}

[[nodiscard]] const std::string& Request::getParamValueRef(string_like auto&& searched_key) const {
    if (m_params.has_value())
        for (const auto& [key, value] : m_params.value())
            if (key == searched_key)
                return value;
    throw std::runtime_error{"There was no key in params called" + std::string{searched_key}};
}

[[nodiscard]] std::string& Request::getParamValueRef(string_like auto&& searched_key) {
    if (m_params.has_value())
        for (auto& [key, value] : m_params.value())
            if (key == searched_key)
                return value;
    throw std::runtime_error{"There was no key in params called" + std::string{searched_key}};
}

Request::Request(Method method, string_like auto&& uri, string_like auto&& protocol)
    : Message{}, m_method{method}, m_uri{std::forward<decltype(uri)>(uri)},
    m_params{}, m_protocol{std::forward<decltype(protocol)>(protocol)}
{
    init();
}

Request::Request(Method method, string_like auto&& uri, Params params, string_like auto&& protocol)
    : Message{}, m_method{method}, m_uri{std::forward<decltype(uri)>(uri)},
    m_params{params}, m_protocol{std::forward<decltype(protocol)>(protocol)}
{
    init();
}

Request::Request(string_like auto&& request_str) {
    auto lines = split(request_str, newLine);
    auto start_line_parts = split(lines[0], " ");
    m_method = fromString(start_line_parts[0]);
    auto urn_parts = split(start_line_parts[1], "?");
    if (urn_parts.size() == 2) {
        auto params_key_value = split(urn_parts[1], "&");
        for (auto &param_key_value : params_key_value)
        {
            auto key_value = split(param_key_value, "=");
            addParam(key_value[0], key_value[1]);
        }
    }
    m_uri = urn_parts[0];
    parseBody(std::move(lines));
    // No init as m_raw message is already initialized
}

void Request::setMethod(Method method) {
    m_method = method;
    init();
}

void Request::setUri(string_like auto&& uri) {
    m_uri = std::forward<decltype(uri)>(uri);
    init();
}

[[nodiscard]] auto Request::getParamValue(string_like auto&& key) const -> std::string {
    return getParamValueRef(std::forward<decltype(key)>(key));
}

[[nodiscard]] auto Request::getParamValueView(string_like auto&& key) const -> std::string_view {
    return getParamValueRef(std::forward<decltype(key)>(key));
}

[[nodiscard]] auto Request::getParams() const -> Params {
    return m_params.has_value() ? m_params.value() : Params{};
}

[[nodiscard]] auto Request::getParamsView() const -> ParamsView {
    return toVecOfStrView(m_params.has_value() ? m_params.value() : Params{});
}

void Request::addParam(string_like auto&& key, string_like auto&& value) {
    if (!m_params.has_value()) {
        m_params.emplace().emplace_back(std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
    }
    else {
        m_params.value().emplace_back(std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
    }
    init();
}

void Request::addParam(same_as<Param> auto&& param) {
    if (!m_params.has_value()) {
        m_params.emplace().push_back(std::forward<decltype(param)>(param));
    }
    else {
        m_params.value().push_back(std::forward<decltype(param)>(param));
    }
    init();
}

void Request::setProtocol(string_like auto&& protocol) {
    m_protocol = std::forward<decltype(protocol)>(protocol);
    init();
}

} // namespace HTTP
