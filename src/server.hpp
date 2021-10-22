#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>

#include <iostream>
#include <string>
#include <stdexcept>
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
    std::function<Response(Request)> handler;

    constexpr bool match(const Request& req) const {
        return uri == req.uri && method == req.method;
    }
};

// Logging
Logger logger{};
const char *START_MESSAGE = "\x1b[32m[Server started]\x1b[0m\n";
const char *NEW_REQ_MESSAGE = "\x1b[32m[New request]\x1b[0m\n";
const char *SENDING_RES_MESSAGE = "\x1b[32m[Sending response]\x1b[0m\n";
const char *ERROR_MESSAGE = "\x1b[31m[Error]\x1b[0m\n";
const char *ACCEPTED_MESSAGE = "\x1b[32m[Accepted client]\x1b[0m\n";
const char *RECIEVED_MESSAGE = "\x1b[32m[Recieved]\x1b[0m\n";
const char *STOP_MESSAGE = "\n\x1b[31m[STOPPING SERVER]\x1b[0m\n";

class Server {
    constexpr static size_t CONNMAX = 100;
    constexpr static size_t CONNMAXPERSOCK = 100;
    constexpr static size_t BUFSIZE = 65535;

    const std::string m_port;
    std::vector<Route> m_routes;

    int m_socketfd;
    size_t m_cur_client = 0;
    std::array<int, CONNMAX> m_clients;
    std::vector<char> m_buffer;
    bool is_running;

    void setup();
    void respond(size_t n);

public:

    template<typename... Routes>
    Server(uint16_t port_, Routes&&... routes_)
        : m_port{std::to_string(port_)}, m_routes{sizeof...(Routes)}, m_buffer(BUFSIZE, 0), is_running{true}
    {
        (m_routes.push_back(std::forward<Routes>(routes_)), ...);
        is_running = true;
        setup();
    }

    void start();
    void stop();
};

void Server::setup()
{
    std::ranges::fill(m_clients, -1);

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo *res;
    if (getaddrinfo(nullptr, m_port.c_str(), &hints, &res) != 0)
    {
        perror("getaddrinfo() error");
        exit(1);
    }

    // socket and bind
    addrinfo* p;
    for (p = res; p != nullptr; p = p->ai_next)
    {
        int option;
        m_socketfd = socket(p->ai_family, p->ai_socktype, 0);
        setsockopt(m_socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (m_socketfd == -1)
            continue;
        if (bind(m_socketfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }
    if (p == nullptr)
    {
        perror("Socket creation or binding error");
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(m_socketfd, CONNMAXPERSOCK) != 0)
    {
        perror("listen error");
        exit(1);
    }
}

void Server::start()
{
    logger.log(START_MESSAGE, "http://127.0.0.1:", m_port.c_str());

    while (is_running)
    {
        sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        m_clients[m_cur_client] = accept(m_socketfd, (sockaddr *)&clientaddr, &addrlen);

        if (m_clients[m_cur_client] == -1)
        {
            if (is_running) perror("accept() error");
            continue;
        }

        logger.log(ACCEPTED_MESSAGE, m_cur_client);

        respond(m_cur_client);
        while (m_clients[m_cur_client] != -1) {
            m_cur_client = (m_cur_client + 1) % CONNMAX;
        }
    }
}

void Server::respond(size_t n)
{
    int clientfd = m_clients[n];

    if (int rcvd = recv(clientfd, m_buffer.data(), BUFSIZE - 1, 0); rcvd < 0)
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0)
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    else
    {
        logger.log(RECIEVED_MESSAGE, rcvd, " bytes from client ", n);

        // Handling received message
        Request req{m_buffer.data()};

        logger.log(NEW_REQ_MESSAGE, req.toString());

        Response response{};
        for (const auto& route : m_routes) {
            if (route.match(req)) {
                try {
                    response = route.handler(req);
                }
                catch (std::exception& e) {
                    logger.log(ERROR_MESSAGE, "Request handler thrown an exeption: ", e.what());
                    response = Response{"HTTP/1.1", 500, "Internal Server Error"};
                }
                break;
            }
        }
        auto response_str = response.toString();

        logger.log(SENDING_RES_MESSAGE, response_str);

        write(clientfd, response_str.c_str(), response_str.length());

        shutdown(clientfd, SHUT_RDWR);
        close(clientfd);
    }
    m_clients[n] = -1;

    std::ranges::fill(m_buffer, '\0');
}

void Server::stop() {
    logger.log(STOP_MESSAGE);
    is_running = false;
    shutdown(m_socketfd, SHUT_RDWR);
    close(m_socketfd);
}

// Support for std::signal
struct ServerStopper {
    static std::function<void()> teardown__;

    static void init(Server& server_instance) {
        teardown__ = [&]{ server_instance.stop(); };
    }
    static void stop(int) {
        teardown__();
    }
};

std::function<void()> ServerStopper::teardown__ = []{};
