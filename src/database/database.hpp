#pragma once

#include <vector>
#include <algorithm>

#include "user.hpp"

class Database
{
    std::vector<std::string> getLines();

    std::vector<User> users;
    const std::string path;

public:
    Database(std::string db_file);
    ~Database();

    bool isPresent(const User &) const;
    bool isRegistered(std::string_view login) const;
    bool rightPassword(std::string_view login, std::string_view pass) const;
    User& addUser(User);
    User& addUser(std::string_view login, std::string_view pass, std::string_view role);
    User& getUser(std::string_view login);
    void deleteUser(const User &);
    void setPassword(User, std::string new_password);
};


void Database::setPassword(User current_user, std::string new_password){
    for (auto& it : users)
    {
        if (it == current_user)
        {
            it.password = new_password;
        }
        break;
    }
}

void Database::deleteUser(const User &current_user)
{
    users.erase(std::find(users.begin(), users.end(), current_user), users.end());
}

bool Database::isPresent(const User &new_user) const
{
    return std::find(users.begin(), users.end(), new_user) != users.end();
}

bool Database::isRegistered(std::string_view login) const {
    return std::find_if(users.begin(), users.end(), [&](auto user){return user.login == login;}) != users.end();
}

bool Database::rightPassword(std::string_view login, std::string_view pass) const {
    return std::find_if(users.begin(), users.end(), [&](auto user){return user.login == login && user.password == pass;}) != users.end();
}

User& Database::addUser(User new_user)
{
    return users.emplace_back(std::move(new_user));
}

User& Database::addUser(std::string_view login, std::string_view pass, std::string_view role)
{
    return users.emplace_back(std::move(std::string{login}), std::move(std::string{pass}), std::move(std::string{role}));
}

User& Database::getUser(std::string_view login) {
    return *std::find_if(users.begin(), users.end(), [&](auto user){return user.login == login;});
}

Database::Database(std::string db_file) : path{db_file}
{
    for (auto line : getLines())
        users.emplace_back(line);
}

std::vector<std::string> Database::getLines()
{
    std::vector<std::string> temp;
    std::ifstream file(path);
    while (!file.eof())
    {
        std::string line;
        std::getline(file, line);
        temp.push_back(line);
    }
    file.close();
    return temp;
}

Database::~Database(){
    std::ofstream file(path);
    for (auto it : users) {
        if (it != users.back())
            file << it.getStrRepr() << '\n';
        else
            file << it.getStrRepr();
    }
    file.close();
}
