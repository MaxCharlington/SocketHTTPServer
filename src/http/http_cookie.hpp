#pragma once

#include <map>
#include <utility>
#include <string_view>
#include <ranges>
#include <cassert>

#include "http_header.hpp"
#include "string_helper.hpp"
#include "logger.hpp"


// Partly specified cookie implementation
// Has support for simplest usecase of cookie

namespace HTTP {

using Cookie = std::pair<std::string_view, std::string_view>;

class Cookies {
    static Logger<> logger;

    std::map<std::string_view, std::string_view> m_cookies;

public:
    Cookies() = default;
    Cookies(std::string_view cookie_header_value) {
        constexpr std::string_view value_delim{"="};
        constexpr std::string_view pair_delim{"; "};

        std::string_view key;
        size_t start = 0;
        size_t pos = 0;

        for (; pos < cookie_header_value.length(); pos++)
        {
            if (std::string_view{&cookie_header_value[pos], value_delim.length()} == value_delim) {
                key = std::string_view{cookie_header_value.data() + start, pos - start};
                start = pos + value_delim.length();
            }
            if (std::string_view{&cookie_header_value[pos], pair_delim.length()} == pair_delim) {
                m_cookies[key] = std::string_view{cookie_header_value.data() + start, pos - start};
                start = pos + pair_delim.length();
            }
        }
        m_cookies[key] = std::string_view{cookie_header_value.data() + start, pos - start};
    }

    Cookies(std::initializer_list<decltype(m_cookies)::value_type> il) : m_cookies{il} {}

    std::string getCookie(std::string_view key) const {
        return std::string{m_cookies.at(key)};
    }

    void setCookie(std::string_view key, std::string_view value) {
        m_cookies[key] = value;
    }

    void setCookie(Cookie cookie) {
        m_cookies[cookie.first] = cookie.second;
    }

    Header getCookieHeader() const {
        std::string cookies_value;
        if (m_cookies.size() > 0) {
            for (const auto& [key, value] : m_cookies)
                cookies_value += std::string{key} + "=" + std::string{value} + "; ";
            cookies_value.resize(cookies_value.size() - 2);  // Remove last '; '
            return {"Cookie", cookies_value};
        }
        return {};
    }

    static Header getSetCookieHeader(std::string key, std::string value) {
        return {"SetCookie", key + '=' + value};
    }


    static Cookie getCookieFromSetHeader(std::string_view cookie_set_header_value) {
        constexpr std::string_view value_delim{"="};
        constexpr std::string_view pair_delim{"; "};

        size_t start = 0;
        size_t pos = 0;
        std::string_view key;

        for (; pos < cookie_set_header_value.length(); pos++)
        {
            if (std::string_view{&cookie_set_header_value[pos], value_delim.length()} == value_delim) {
                key = std::string_view{cookie_set_header_value.data() + start, pos - start};
                start = pos + value_delim.length();
            }
            if (std::string_view{&cookie_set_header_value[pos], pair_delim.length()} == pair_delim) {
                return {key, std::string_view{cookie_set_header_value.data() + start, pos - start}};
            }
        }
        return {key, std::string_view{cookie_set_header_value.data() + start, pos - start}};
    }
};

Logger<> Cookies::logger{};

}  // namespace HTTP
