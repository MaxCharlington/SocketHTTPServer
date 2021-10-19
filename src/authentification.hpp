#pragma once

#include <unordered_map>
#include <chrono>
#include <functional>

#include "database/database.hpp"
#include "database/user.hpp"

class Auth {
    static size_t getUniqueKey() {
        auto current_time = std::time(0);
        auto str = ctime(&current_time);
        return std::hash<std::string>{}(str);
    }

    Database& m_db;
    std::unordered_map<size_t, const User&> m_auth_users;

public:
    Auth(Database& db) : m_db{db} {}

    bool isAuthorized(size_t id) const {
        return m_auth_users.count(id) == 1;
    }

    size_t authorize(const User& user) {
        if (m_db.isPresent(user)) {
            auto id = getUniqueKey();
            m_auth_users.emplace(id, user);
            return id;
        }
        throw std::runtime_error{"Not registered user"};
    }
};
