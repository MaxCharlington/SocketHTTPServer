#pragma once

#include <string_view>

template <typename KeyT = std::string, typename ValueT = std::string>
struct Header
{
    using key_type = KeyT;
    using value_type = ValueT;

    KeyT key;
    ValueT value;

    Header() = default;
    Header(key_type key_, value_type value_) : key{key_}, value{value_} {}
};


using Header_t = Header<>;
using Headers = std::vector<Header_t>;

std::string_view getHeaderImpl(const Headers& headers, std::string_view key) {
    for (auto& header : headers) {
        if (header.key == key)
            return header.value;
    }
    return "";
}
