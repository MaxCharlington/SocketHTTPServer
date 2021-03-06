#include <string>
#include <csignal>

#include "server.hpp"
#include "database/database.hpp"
#include "authentification.hpp"

using namespace std::string_literals;
using namespace http;
using enum HttpMethod;


int main()
{
    Database db{"db.txt"};
    Auth auth{db};

    Server s{3000,
        Route{
            "/register", GET,
            [&](auto req){
                auto login = req.getParamValue("login");
                auto pass = req.getParamValue("pass");
                auto role = req.getParamValue("role");
                auto new_user = User{login, pass, role};
                if (!db.isPresent(new_user.login)) {
                    Response res{};
                    User& user = db.addUser(new_user);
                    auto id = auth.authorize(user);
                    res = Response{"HTTP/1.1", 200};
                    res.addHeader(Cookies::getSetCookieHeader("id", std::to_string(id)));
                    return res;
                }
                return Response{};
            }
        },
        Route{
            "/login", GET,
            [&](auto req){
                auto login = req.getParamValue("login");
                auto pass = req.getParamValue("pass");
                if (db.rightPassword(login, pass)) {
                    Response res{};
                    User& user = db.getUser(login);
                    auto id = auth.authorize(user);
                    res = Response{"HTTP/1.1", 200};
                    res.addHeader(Cookies::getSetCookieHeader("id", std::to_string(id)));
                    return res;
                }
                return Response{};
            }
        },
        Route{
            "/data", GET,
            [&](auto req){
                Cookies cookies{req.getHeaderValue("Cookie")};
                auto id = std::stoull(cookies.getCookie("id"));
                if (auth.isAuthorized(id)) {
                    return Response{"Some very useful data"s};
                }
                return Response{"HTTP/1.1", 401};
            }
        }
    };

    ServerStopper::init(s);
    std::signal(SIGINT, ServerStopper::stop);

    s.start();
}

