#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>

#include <string_view>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include <ranges>


template <typename KeyT = const char*, typename ValueT = const char*>
struct Header
{
    using key_type = KeyT;
    using value_type = ValueT;

    KeyT key;
    ValueT value;

    Header() = default;
    Header(key_type key_, value_type value_) : key{key_}, value{value_} {}
};


struct Request {
    using Header_t = Header<>;

    char* method;    // "GET" or "POST"
    char* uri;       // "/index.html" things before '?'
    char* params;    // "a=1&b=2"     things after  '?'
    char* protocol;  // "HTTP/1.1"
    std::vector<Header_t> headers;
    const char *payload; // for POST
    int payload_size;

    Request() = default;
    Request(char* req);

    Request::Header_t::value_type get_header(std::string_view key);
};

Request::Request(char* req) {
    method = strtok(req, " \t\r\n");
    uri = strtok(nullptr, " \t");
    protocol = strtok(nullptr, " \t\r\n");

    fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);

    if (params = strchr(uri, '?'); params)
        *params++ = '\0'; //split URI
    else
        params = uri - 1; //use an empty string

    char *t;
    while (true)
    {
        char* key = strtok(nullptr, "\r\n: \t");
        if (!key)
            break;
        char* value = strtok(nullptr, "\r\n");
        while (*value && *value == ' ')
            value++;
        headers.emplace_back(key, value);
        fprintf(stderr, "\t[H] %s: %s\n", key, value);
        t = value + 1 + strlen(value);
        if (t[1] == '\r' && t[2] == '\n')
            break;
    }
    t++;  // now the *t shall be the beginning of user payload
    const char* t2 = get_header("Content-Length"); // and the related header if there is
    payload = t;
    //payload_size = t2 ? atol(t2) : (rcvd - (t - buf));
    payload_size = t2 ? atol(t2) : 0;  // TMP
}

Request::Header_t::value_type Request::get_header(std::string_view key) {
    for (auto header : headers) {
        if (header.key == key)
            return header.value;
    }
    return Header_t::value_type{};
}


struct Route {
    std::string_view uri;
    std::string_view method;
    std::function<void(Request)> handler;

    constexpr bool match(const Request& req) const {
        return uri == req.uri && method == req.method;
    }
};


class Server {
    constexpr static size_t CONNMAX = 100;
    constexpr static size_t CONNMAXPERSOCK = 100;
    constexpr static size_t BUFSIZE = 65535;

    const std::string port;
    std::vector<Route> routes;

    int listenfd;
    std::array<int, CONNMAX> clients;  
    std::vector<char> buf;
    
    void setup();
    void respond(size_t n);

public:
    template<typename... Routes>
    Server(uint16_t port_, Routes... routes_) : port{std::to_string(port_)}, routes{sizeof...(Routes)}, buf(BUFSIZE, 0) {
        (routes.push_back(std::forward<Routes>(routes_)), ...);
        setup();
    }

    void start();
};

void Server::setup()
{
    std::ranges::fill(clients, -1);
    
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo *res;
    if (getaddrinfo(nullptr, port.c_str(), &hints, &res) != 0)
    {
        perror("getaddrinfo() error");
        exit(1);
    }

    // socket and bind
    addrinfo* p;
    for (p = res; p != nullptr; p = p->ai_next)
    {
        int option;
        listenfd = socket(p->ai_family, p->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (listenfd == -1)
            continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }
    if (p == nullptr)
    {
        perror("Socket creation or binding error");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if (listen(listenfd, CONNMAXPERSOCK) != 0)
    {
        perror("listen error");
        exit(1);
    }
}

// void Server::recieve() {

// }

void Server::respond(size_t n)
{
    Request req{};
    int clientfd;
    int rcvd = recv(clients[n], buf.data(), BUFSIZE - 1, 0);

    if (rcvd < 0)
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0)
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    else 
    {
        // Handling received message
        buf[rcvd] = '\0';
        req = Request{buf.data()};

        fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", req.method, req.uri);

        // bind clientfd to stdout, making it easier to write
        clientfd = clients[n];
        dup2(clientfd, STDOUT_FILENO);
        close(clientfd);

        for (const auto & route : routes) {
            if (route.match(req)) {
                route.handler(req);
                break;
            }
        }

        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
    }

    shutdown(clientfd, SHUT_RDWR); //All further send and recieve operations are DISABLED...
    close(clientfd);
    clients[n] = -1;
}

void Server::start()
{
    printf("Server started \033[92mhttp://127.0.0.1:%s\033[0m\n", port.c_str());

    signal(SIGCHLD, SIG_IGN);  // Ignore SIGCHLD to avoid zombie threads

    size_t slot = 0;
    while (true)
    {
        sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        clients[slot] = accept(listenfd, (sockaddr *)&clientaddr, &addrlen);

        if (clients[slot] < 0)
        {
            perror("accept() error");
        }
        else if (fork() == 0)  // Multithreaded
        {
            respond(slot);
            exit(0);
        }

        while (clients[slot] != -1) {
            slot = (slot + 1) % CONNMAX;
        }
    }
}
