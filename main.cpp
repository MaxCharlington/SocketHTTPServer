#include <string>

#include "src/server.hpp"
#include "src/database/database.hpp"
#include "src/authentification.hpp"

using namespace std::string_literals;
using enum HTTPMethod;

int main()
{
    Database db{"db.txt"};
    Auth auth{db};

    Server s{3000,
        Route{
            "/register", GET,
            []([[maybe_unused]] auto req){
                return "";
            }
        },
        Route{
            "/login", GET,
            []([[maybe_unused]] auto req){
                return "";
            }
        },

        // Sample routes
        Route{
            "/test", GET,
            [](auto req){
                return "Hello! You are using "s + req.getHeader("User-Agent") + "\r\n" + "Param 'name' was: " + req.getParam("name");
            }
        },
        Route{
            "/test", POST,
            [](auto req){
                Cookies cookies{req.getHeader("Cookie")};
                auto name = cookies.getCookie("name");

                auto res = "Wow, seems that you POSTed " + std::to_string(req.content.length()) + "bytes. \r\n";
                res += "Content was: " + req.content + "\r\n";
                res += "Cookie 'name' was: " + name;
                return res;
            }
        }
    };
    s.start();
}
