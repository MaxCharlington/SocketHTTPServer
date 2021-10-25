#include <iostream>
#include "http_responce.hpp"

int main() {
    Response r{};
    std::cout << r.toString() << '\n';
    Response r2{Response::DEFAULT_PROTOCOL, 301};
    std::cout << r2.toStringView() << '\n';
    Response r3{"HTTP/2 200 OK\r\rContent-Length: 2\r\nContent-Type: plain/text\r\n\r\nOK"};
    std::cout << r3.getHeaderValue("Content-Type");
}
