#include <iostream>
#include "http_request.hpp"

using namespace HTTP;
using enum HTTPMethod;

int main() {
    // Request r{};
    // std::cout << r.toString() << '\n';
    Request r2{GET, "/animal/cat", {{"leg", "4"}, {"ear", "2"}}, "HTTP/2"};
    std::cout << r2.toStringView() << "  Ears: " << r2.getParamValueView("ear") << "\n";
    Request r3{"GET /favicon.ico HTTP/1.1\r\nHost: localhost:3000\r\nConnection: keep-alive\r\nsec-ch-ua: \"Microsoft Edge\";v=\"95\", \"Chromium\";v=\"95\", \";Not A Brand\";v=\"99\"\r\nsec-ch-ua-mobile: ?0\r\n"
               "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.54 Safari/537.36 Edg/95.0.1020.30\r\nsec-ch-ua-platform: \"Windows\"\r\nAccept: image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n"
               "Sec-Fetch-Site: same-origin\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Dest: image\r\nReferer: http://localhost:3000/\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: ru,en;q=0.9,en-GB;q=0.8,en-US;q=0.7"};
    std::cout << r3.getHeaderValue("Sec-Fetch-Mode");
}
