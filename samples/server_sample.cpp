#include <string>
#include <csignal>

#include "server.hpp"

using namespace std::string_literals;
using enum HttpMethod;


int main()
{
    Server s{3000,
        Route{
            "/", GET,
            [&](Request req){
                auto res = Response{"HTTP/1.1", 200};
                res.setContent("Your user agent is: " + req.getHeaderValue("User-Agent"));
                return res;
            }
        },
        Route{
            "/", POST,
            [&]([[maybe_unused]] auto req){
                return Response{"HTTP/1.1", 200};
            }
        }
    };

    ServerStopper::init(s);
    std::signal(SIGINT, ServerStopper::stop);

    s.start();
}
