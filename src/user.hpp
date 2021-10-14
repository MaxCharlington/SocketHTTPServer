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

    User(std::string db_line); // +

    std::string getStrRepr() {
        return username + ' ' + password + ' ' + role + '\n';
    }

    //--------------------------------------------

    void copyFile(const std::string &file_name) // -
    {
        std::ifstream file(file_name);
        while (!file.eof())
        {
            std::string line;
            std::getline(file, line);
            fileLines.push_back(line);
        }
        file.close();
    }

    auto isPresent() // -
    {
        std::string temp;
        for (std::string it : fileLines)
        {
            for (auto ch : it)
            {
                if (ch != ' ')
                {
                    temp+=ch;
                }
                else{
                    break;
                }
            }
            if (this->username == temp)
            {
                return true;
            }
        }
        return false;
    }
};
