#include <iostream>

#include "src/client.hpp"

using enum HTTPMethod;

int main() {
    Client client{};

    Request req{GET, "/test"};

    client.request(req);

    auto res = client.getResponce();

    std::cout << res.toString();

    Request req2{POST, "/"};
    req2.addHeader("Cookie", "name=Max; surname=Charlington");
    req2.addContent("Some content");

    client.request(req2);

    res = client.getResponce();

    std::cout << res.toString();

}
