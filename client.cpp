#include <iostream>

#include "src/client.hpp"

using enum HTTPMethod;

int main() {
    Client client{};

    Request req{POST, "/"};
    req.headers.emplace_back("User-Agent", "ConsoleApplication");  // Add method
    
    req.addContent("Some content");
    client.request(req);

    auto res = client.getResponce();

    Request req2{GET, "/"};
    client.request(req);

    res = client.getResponce();
}
