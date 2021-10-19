#pragma once

#include <map>
#include <string_view>
#include <ranges>
#include <cassert>

#include "http_header.hpp"
#include "../logger.hpp"

// Partly specified cookie implementation
// Has support for simplest usecase of cookie

class Cookie {
    static Logger<> logger;
    std::map<std::string_view, std::string_view> m_cookies;

public:
    using Header_t = Header<>;

    Cookie(std::string_view cookie_header_value) {
        constexpr std::string_view value_delim{"="};
        constexpr std::string_view pair_delim{"; "};

        std::string_view key;
        std::string_view value;
        size_t start = 0;
        size_t pos = 0;

        for (; pos < cookie_header_value.length(); pos++)
        {
            if (std::string_view{&cookie_header_value[pos], value_delim.length()} == value_delim) {
                key = std::string_view{cookie_header_value.data() + start, pos - start};
                start = pos + value_delim.length();
            }
            if (std::string_view{&cookie_header_value[pos], pair_delim.length()} == pair_delim) {
                value = std::string_view{cookie_header_value.data() + start, pos - start};
                m_cookies[key] = value;
                start = pos + pair_delim.length();
            }
        }
        value = std::string_view{cookie_header_value.data() + start, pos - start};
        m_cookies[key] = value;
    }

    std::string getCookie(std::string_view key) const {
        auto val = m_cookies.at(key);

        return std::string{val};
    }
};

Logger<> Cookie::logger{};
