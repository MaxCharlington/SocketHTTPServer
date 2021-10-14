#pragma once

#include <fstream>
#include <vector>
struct User
{
    std::string username;
    std::string password;
    std::string role;
    std::vector<std::string> fileLines;

    void copyFile(const std::string &file_name)
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

    auto isPresent()
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

    void writeToDb(std::string file)
    {
        std::ofstream fout(file);
        if (fout.is_open())
        {
            if (isPresent())
            {
                fout << this->username << " " << this->password << " " << this->role << std::endl;
            }
        }
        fout.close();
    }
};
