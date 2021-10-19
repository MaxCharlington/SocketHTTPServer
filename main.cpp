#include <string>

#include "src/server.hpp"
#include "src/database/database.hpp"

using namespace std::string_literals;
using enum HTTPMethod;

int main()
{
    Server s{3000,
        Route{
            "/", GET,
            [](auto req){
                return "Hello! You are using "s + req.getHeader("User-Agent");
            }
        },
        Route{
            "/test", GET,
            []([[maybe_unused]] auto req){
                return "";
            }
        },
        Route{
            "/", POST,
            [](auto req){
                Cookie cookie{req.getHeader("Cookie")};
                auto name = cookie.getCookie("name");

                auto res = "Wow, seems that you POSTed " + std::to_string(req.content.length()) + "bytes. \r\n";
                res += "Content was: " + req.content + "\r\n";
                res += "Cookie 'name' was: " + name;
                return res;
            }
        }
    };
    s.start();
}
