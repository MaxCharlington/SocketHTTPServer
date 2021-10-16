#include <iostream>

#include "src/client.hpp"

using enum HTTPMethod;

int main() {
    Client client{};

    Request req{POST, "/"};
    req.addHeader("User-Agent", "ConsoleApplication");
    
    req.addContent("Some content");
    client.request(req);

    auto res = client.getResponce();

    std::cout << res.toString();

    Request req2{GET, "/"};
    client.request(req);

    res = client.getResponce();

    std::cout << res.toString();
}
