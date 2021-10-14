#pragma once

#include <vector>

#include "user.hpp"

class Database {
    std::vector<User> users;

    std::vector<std::string> getLines(std::string db_file); // + из User::copyFile

public:
    Database(std::string db_file); // Заполняет внутренний вектор из файла
    ~Database(); // Записывает содержимое вектора в файл  // +

    // Работают со внутренним вектором!!!!!
    bool isPresent(const User&);// +
    void addUser(User);// +
    void deleteUser(const User&);// +
    void setPassword(User, std::string new_password);// +
};

/*
имя0 хэшпароля0 маркер0
имя1 хэшпароля1 маркер1
имя2 хэшпароля2 маркер2
имя3 хэшпароля3 маркер3
*/

Database::Database(std::string db_file) {
    for (auto line : getLines(db_file)) {
        users.emplace_back(line);
    }
}

Database::~Database() {
    // очистить файл
    for (auto user : users) {
        // записать в файл user.getStrRepr()
    }
}
