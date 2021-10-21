#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cassert>

#include "http/http.hpp"
#include "logger.hpp"

// Logging
Logger logger{};
const char *CREATE_REQ_MESSAGE = "\x1b[32m[Sending new request]\x1b[0m\n";
const char *SENT_MESSAGE = "\x1b[32m[Sent]\x1b[0m\n";
const char *GOT_RES_MESSAGE = "\x1b[32m[Got response]\x1b[0m\n";
const char *NEW_COOKIE_MESSAGE = "\x1b[32m[Got new cookie]\x1b[0m\n";

class Client {
    constexpr static size_t BUFSIZE = 65535;
    constexpr static uint16_t PORT = 3000;

    int sock = 0;
    sockaddr_in serv_addr;
    std::vector<char> buf;
    Cookies cookies;

    void setup();
    void teardown();

public:
    Client() : buf(BUFSIZE, 0) {}

    void request(Request req);
    Response getResponce();

};

void Client::setup() {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Socket creation error \n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address/ Address not supported \n");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "Connection Failed \n");
        exit(1);
    }
}

void Client::teardown() {
    shutdown(sock, SHUT_RDWR);
    close(sock);
}

void Client::request(Request req) {
    req.addHeader(cookies.getCookieHeader());
    auto message = req.toString();

    setup();

    logger.log(CREATE_REQ_MESSAGE, message);

    size_t send_len = send(sock, message.c_str(), message.length(), 0);
    assert(send_len > 0);

    logger.log(SENT_MESSAGE, send_len, " bytes");

    size_t read_len = read(sock, buf.data(), buf.size() - 1);
    assert(read_len > 0);

    buf[read_len] = '\0';

    teardown();
}

Response Client::getResponce() {
    Response res{buf.data()};

    logger.log(GOT_RES_MESSAGE, res.toString());

    auto cookie_to_save = res.getHeader("SetCookie");
    if (cookie_to_save.length() > 0) {
        Cookie c = Cookies::getCookieFromSetHeader(cookie_to_save);

        logger.log(NEW_COOKIE_MESSAGE, c.first, c.second);

        cookies.setCookie(c);
    }

    return res;
}
