#include <iostream>

#include "src/client.hpp"

using enum HTTPMethod;

int main() {
    Client client{};

    Request req{GET, "/register?login=Maxim&pass=Maxim&role=coolGuy"};  // Add setParam method
    client.request(req);
    auto res = client.getResponce();

    Request req2{GET, "/data"};
    client.request(req2);

    res = client.getResponce();

    std::cout << res.toString();
}
