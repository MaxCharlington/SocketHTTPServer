#pragma once

#include <fstream>
#include <vector>

struct User
{
    std::string username;
    std::string password;
    std::string role;

    User(std::string username_, std::string password_, std::string role_)
        : username{std::move(username_)}, password{std::move(password_)}, role{std::move(role_)} {}

    User(std::string db_line);

    std::string getStrRepr()
    {
        return username + ' ' + password + ' ' + role;
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
                username += it;
            else if (infoCounter == 1)
                password += it;
            else
                role += it;
        }
        else infoCounter++;
    }
};
