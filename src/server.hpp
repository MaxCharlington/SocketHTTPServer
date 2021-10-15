#pragma once

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

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include <ranges>

#include "http/http.hpp"
#include "logger.hpp"

struct Route {
    std::string_view uri;
    HTTPMethod method;
    std::function<std::string(Request)> handler;

    constexpr bool match(const Request& req) const {
        return uri == req.uri && method == req.method;
    }
};

// Logging
Logger logger{};
const char *NEW_REQ_MESSEGE = "\x1b[32m[New request]\x1b[0m\n";
const char *CREATE_RES_MESSEGE = "\x1b[32m[Sending response]\x1b[0m\n";

class Server {
    constexpr static size_t CONNMAX = 100;
    constexpr static size_t CONNMAXPERSOCK = 100;
    constexpr static size_t BUFSIZE = 65535;

    const std::string port;
    std::vector<Route> routes;

    int listenfd;
    size_t cur_client = 0;
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
        int option{1};
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

    if (listen(listenfd, CONNMAXPERSOCK) != 0)
    {
        perror("listen error");
        exit(1);
    }
}

void Server::respond(size_t n)
{
    int clientfd = clients[n];
    if (int rcvd = recv(clientfd, buf.data(), BUFSIZE - 1, 0); rcvd < 0)
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0)
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    else 
    {
        // Handling received message
        buf[rcvd] = '\0';
        Request req{buf.data()};

        logger.log(NEW_REQ_MESSEGE, req.to_string());

        Response response{"HTTP/1.1", 500, "Internal Server Error"};
        for (const auto& route : routes) {
            if (route.match(req)) {
                try {
                    response = Response{"HTTP/1.1", 200, "OK"};
                    response.addContent(route.handler(req));
                }
                catch (...) {}
                break;
            }
        }
        auto response_str = response.to_string();

        logger.log(CREATE_RES_MESSEGE, response_str);

        write(clientfd, response_str.c_str(), response_str.length());

        shutdown(clientfd, SHUT_RDWR);
        close(clientfd);
    }
    clients[n] = -1;
}

void Server::start()
{
    printf("Server started \033[92mhttp://127.0.0.1:%s\033[0m\n", port.c_str());

    signal(SIGCHLD, SIG_IGN);  // Ignore SIGCHLD to avoid zombie threads

    while (true)
    {
        sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        clients[cur_client] = accept(listenfd, (sockaddr *)&clientaddr, &addrlen);

        if (clients[cur_client] == -1)
        {
            perror("accept() error");
        }
        else if (fork() == 0)
        {
            // New process
            respond(cur_client);
            exit(0);
        }
        else {
            // Current process
            while (clients[cur_client] != -1) {
                cur_client = (cur_client + 1) % CONNMAX;
            }
        }
    }
}
