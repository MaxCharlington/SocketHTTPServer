#include <string>

#include "src/server.hpp"

using namespace std::string_literals;
using enum HTTPMethod;

int main()
{
    Server s{3000,
        Route{
            "/", GET,
            [](auto req){
                return "Hello! You are using "s + req.get_header("User-Agent");
            }
        },
        Route{
            "/", POST,
            [](auto req){
                auto res = "Wow, seems that you POSTed" + std::to_string(req.payload_size) + "bytes. \r\n";
                res += "Fetch the data using `payload` variable.";
                return res;
            }
        }
    };
    s.start();
}
