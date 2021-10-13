#include "src/server.hpp"

int main()
{
    Server s{3000,
        Route{
            "/", "GET",
            [](auto req){
                printf("HTTP/1.1 200 OK\r\n\r\n");
                printf("Hello! You are using %s", req.get_header("User-Agent"));
            }
        },
        Route{
            "/", "POST",
            [](auto req){
                printf("HTTP/1.1 200 OK\r\n\r\n");
                printf("Wow, seems that you POSTed %d bytes. \r\n", req.payload_size);
                printf("Fetch the data using `payload` variable.");
            }
        }
    };
    s.start();
}
