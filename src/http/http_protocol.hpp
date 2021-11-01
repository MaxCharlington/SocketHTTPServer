#pragma once

#include <array>
#include <algorithm>

#include "string_like.hpp"


namespace http {


using namespace std::literals;

const std::array protocol_versions = {"HTTP/1.0"sv, "HTTP/1.1"sv, "HTTP/2"sv, "HTTP/3"sv};

[[nodiscard]] bool isValid(string_like auto&& protocol) {
    return std::ranges::any_of(
        protocol_versions,
        [protocol](auto&& prot){ return prot == protocol; }
    );
}


} // namespace http
