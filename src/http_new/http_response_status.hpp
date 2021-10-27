#pragma once

#include <string_view>
#include <stdexcept>


namespace HTTP {

using HTTPStatus = uint16_t;

constexpr void assertIsValid(HTTPStatus status) {
    switch(status) {
        case 100 ... 103:
        [[likely]] case 200 ... 208:
        case 226:
        case 300 ... 308:
        [[likely]] case 400 ... 419:
        case 421 ... 426:
        case 428 ... 429:
        case 431:
        case 449:
        case 451:
        case 499:
        [[likely]] case 500 ... 511:
        case 520 ... 526:
            return;
    }
    throw std::runtime_error("Invalid HTTP status value" + std::to_string(status));
}

[[nodiscard]] constexpr bool isSuccessful(HTTPStatus status) {
    assertIsValid(status);
    return status >= 200 && status <= 208 || status == 226;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
[[nodiscard]] constexpr std::string_view getHTTPStatusMessage(HTTPStatus status) {
    assertIsValid(status);
    switch (status)
    {
        // 1xx: Informational :
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        case 103: return "Early Hints";
        // 2xx: Success :
        [[likely]] case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        case 207: return "Multi-Status";
        case 208: return "Already Reported";
        case 226: return "IM Used";
        // 3xx: Redirection :
        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Moved Temporarily";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 305: return "Use Proxy";
        [[unlikely]] case 306: return "";  // Reserved
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";
        // 4xx: Client Error :
        [[likely]] case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        [[likely]] case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        [[unlikely]] case 418: return "I’m a teapot";
        case 419: return "Authentication Timeout";
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Entity";
        case 423: return "Locked";
        case 424: return "Failed Dependency";
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 449: return "Retry With";
        case 451: return "Unavailable For Legal Reasons";
        case 499: return "Client Closed Request";
        // 5xx: Server Error :
        [[likely]] case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 506: return "Variant Also Negotiates";
        case 507: return "Insufficient Storage";
        case 508: return "Loop Detected";
        case 509: return "Bandwidth Limit Exceeded";
        case 510: return "Not Extended";
        case 511: return "Network Authentication Required";
        case 520: return "Unknown Error";
        case 521: return "Web Server Is Down";
        case 522: return "Connection Timed Out";
        case 523: return "Origin Is Unreachable";
        case 524: return "A Timeout Occurred";
        case 525: return "SSL Handshake Failed";
        case 526: return "Invalid SSL Certificate";
    }
}
#pragma GCC diagnostic pop

constexpr size_t statusLength = 3;

} // namespace HTTP
