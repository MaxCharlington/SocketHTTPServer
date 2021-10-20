#pragma once

#include <fstream>
#include <vector>

struct User
{
    std::string login;
    std::string password;
    std::string role;

    User(auto username_, auto password_, auto role_)
        : login{std::move(std::string{username_})}, password{std::move(std::string{password_})}, role{std::move(std::string{role_})} {}

    User(std::string db_line);

    std::string getStrRepr()
    {
        return login + ' ' + password + ' ' + role;
    }

    auto operator<=>(const User &) const = default;
};

User::User(std::string db_line)
{
    int infoCounter = 0;
    for (char it : db_line)
    {
        if (it != ' ')
        {
            if (infoCounter == 0)
                login += it;
            else if (infoCounter == 1)
                password += it;
            else
                role += it;
        }
        else infoCounter++;
    }
};
