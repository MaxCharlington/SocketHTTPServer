#pragma once

#include <type_traits>
#include <concepts>
#include <string>
#include <string_view>
#include <vector>

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


std::vector<std::string_view> split(const std::string_view str, std::string_view delimiter) {
    size_t pos_start = 0, pos_end;
    std::vector<std::string_view> res;

    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
        res.push_back(str.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delimiter.length();
    }

    res.push_back(str.substr(pos_start));
    return res;
}


[[nodiscard]] std::vector<std::pair<std::string, std::string>> toVecOfStr(const std::vector<std::pair<std::string_view, std::string_view>>& views) {
    std::vector<std::pair<std::string, std::string>> strs;
    strs.reserve(views.size());
    for (auto [key, value] : views)
        strs.emplace_back(std::string{key}, std::string{value});
    return strs;
}

[[nodiscard]] std::vector<std::pair<std::string_view, std::string_view>> toVecOfStrView(const std::vector<std::pair<std::string, std::string>>& strs) {
    std::vector<std::pair<std::string_view, std::string_view>> views;
    views.reserve(strs.size());
    for (auto [key, value] : strs)
        views.emplace_back(std::string{key}, std::string{value});
    return views;
}
