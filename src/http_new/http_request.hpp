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
    Params m_params;         // "a=1&b=2" things after  '?'
    std::string m_protocol;  // "HTTP/<version>"

    // Internal
    std::string getStartLine() const final;
    auto getParamValueRef(string_like auto&& key) const -> const std::string&;
    auto getParamValueRef(string_like auto&& key) -> std::string&;

public:
    Request(const Request&) = default;
    Request(Request&&) = default;
    Request& operator=(const Request&) = default;
    Request& operator=(Request&&) = default;

    Request(Method method, string_like auto&& uri, string_like auto&& protocol)
        : m_method{method}, m_uri{std::forward<decltype(uri)>(uri)},
        m_params{}, m_protocol{std::forward<decltype(protocol)>(protocol)} {}

    Request(Method method, string_like auto&& uri, Params params, string_like auto&& protocol)
        : m_method{method}, m_uri{std::forward<decltype(uri)>(uri)},
        m_params{params}, m_protocol{std::forward<decltype(protocol)>(protocol)} {}

    Request(string_like auto&& request_str);


    [[nodiscard]] auto getMethod() const -> Method { return m_method; }
    void setMethod(Method method) { m_method = method; }


    template <typename Ret = std::string>
    [[nodiscard]] auto getUri() -> Ret { return m_uri; }

    void setUri(string_like auto&& uri) { m_uri = std::forward<decltype(uri)>(uri); }


    template <typename Ret = std::string>
    [[nodiscard]] auto getParamValue(string_like auto&& key) const -> Ret { return getParamValueRef(std::forward<decltype(key)>(key)); }

    [[nodiscard]] auto getParams() const -> const Params& { return m_params; }

    void addParam(string_like auto&& key, string_like auto&& value) { m_params.insert_or_assign(std::string{std::forward<decltype(key)>(key)}, std::string{std::forward<decltype(value)>(value)}); }

    void addParam(same_as<Param> auto&& param);


    template <typename Ret = std::string>
    [[nodiscard]] auto getProtocol() const -> Ret { return m_protocol; }

    void setProtocol(string_like auto&& protocol) { m_protocol = std::forward<decltype(protocol)>(protocol); }
};


[[nodiscard]] std::string Request::getStartLine() const {
    std::string path_params = m_uri;
    if (m_params.size() > 0) {
        path_params += "?";
        for (const auto&[key, value] : m_params)
        {
            path_params += key + "=" + value + "&";
        }
        path_params.pop_back();
    }
    return std::to_string(std::to_underlying(m_method)) + ' ' + path_params + ' ' + m_protocol;
}

[[nodiscard]] const std::string& Request::getParamValueRef(string_like auto&& searched_key) const {
    if (auto search = m_params.find(searched_key); search != m_params.end())
        return search->second;
    throw std::runtime_error{"There was no key in params called" + std::string{searched_key}};
}

[[nodiscard]] std::string& Request::getParamValueRef(string_like auto&& searched_key) {
    if (auto search = m_params.find(searched_key); search != m_params.end())
        return search->second;
    throw std::runtime_error{"There was no key in params called" + std::string{searched_key}};
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
}

void Request::addParam(same_as<Param> auto&& param) {
    auto&&[key, value] = param;
    m_params.insert_or_assign(std::string{std::forward<decltype(key)>(key)}, std::string{std::forward<decltype(value)>(value)});
}

} // namespace HTTP
