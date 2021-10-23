#pragma once

#include <type_traits>
#include <concepts>
#include <string>
#include <string_view>

#include "helpers.hpp"

using namespace std::string_literals;


template <typename T>
concept Stringlike = []
{
    using type = remove_all_const_t<std::decay_t<T>>;
    return std::is_same_v<type, char *> ||
           std::is_same_v<type, char> ||
           std::is_same_v<type, std::string> ||
           std::is_same_v<type, std::string_view>;
}();

// Hacky way to concat stringlike objects. Update to reduce allocations
template <Stringlike Op1, Stringlike Op2>
std::string operator+(Op1 &&op1, Op2 &&op2)
{
    return std::string{std::forward<Op1>(op1)} + std::string{std::forward<Op2>(op2)};
}


std::string replace(std::string subject, std::string_view search, std::string_view replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

void replace_implace(std::string& subject, std::string_view search, std::string_view replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}


std::string unescapeRequestStr(std::string subject)
{
    replace_implace(subject, "\n", "\x1b[33m\\n\x1b[0m\n");
    replace_implace(subject, "\r", "\x1b[33m\\r\x1b[0m");
    replace_implace(subject, " ", "\x1b[33m_\x1b[0m");
    return subject;
}
