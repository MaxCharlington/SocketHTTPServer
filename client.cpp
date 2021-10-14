#include <iostream>

#include "src/client.hpp"

using enum HTTPMethod;

int main() {
    Client c{};
    Request r{POST, "/"};
    r.headers.emplace_back("User-Agent", "ConsoleApplication");
    r.addContent("Some content");
    c.request(r);
    std::cout << c.getResponce().content;
}
