#include <string>
#include <csignal>

#include "server.hpp"

using namespace std::string_literals;
using enum HTTPMethod;


int main()
{
    Server s{3000,
        Route{
            "/", GET,
            [&](Request req){
                auto res = Response{"HTTP/1.1", 200, "OK"};
                res.addContent("Your user agent is: " + std::string(req.getHeader("User-Agent")));
                return res;
            }
        },
        Route{
            "/", POST,
            [&]([[maybe_unused]] auto req){
                return Response{"HTTP/1.1", 200, "OK"};
            }
        }
    };

    ServerStopper::init(s);
    std::signal(SIGINT, ServerStopper::stop);

    s.start();
}
