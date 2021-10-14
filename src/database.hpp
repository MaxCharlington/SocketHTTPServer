#pragma once

#include <vector>
#include <algorithm>
#include "user.hpp"

class Database
{
    std::vector<User> users;
    std::string path;
    std::vector<std::string> getLines(std::string db_file);

public:
    Database(std::string db_file);
    ~Database();

    bool isPresent(const User &);
    void addUser(User);
    void deleteUser(const User &);
    void setPassword(User, std::string new_password);
};

/*
имя0 хэшпароля0 маркер0
имя1 хэшпароля1 маркер1
имя2 хэшпароля2 маркер2
имя3 хэшпароля3 маркер3
*/
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
    auto it = std::find(users.begin(), users.end(), current_user);
    users.erase(it, users.end());
}

bool Database::isPresent(const User &new_user)
{
    if (std::find(users.begin(), users.end(), new_user) != users.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Database::addUser(User new_user)
{
    if (!isPresent(new_user))
    {
        users.push_back(new_user);
    }
}

Database::Database(std::string db_file)
{
    path = db_file;
    for (auto line : getLines(db_file))
    {
        users.emplace_back(line);
    }
}

std::vector<std::string> Database::getLines(const std::string file_name)
{
    std::vector<std::string> temp;
    std::ifstream file(file_name);
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
    for(auto it: users){              
        if (it != users.back())
        {
            file << it.getStrRepr() << '\n';  
            
        }
        else{
            file << it.getStrRepr(); 
        }
    }
        file.close();
    }


