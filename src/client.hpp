#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#include <cstdio>
#include <unistd.h>
#include <cstring>

#include "http.hpp"

class Client {
    constexpr static size_t BUFSIZE = 65535;
    constexpr static uint16_t PORT = 3000;

    int sock = 0;
    sockaddr_in serv_addr;
    std::vector<char> buf;
    
    void setup();

public:
    Client() : buf(BUFSIZE, 0) {
        setup();
    }

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

void Client::request(Request req) {
    auto message = req.to_string();
    send(sock, message.c_str(), message.length(), 0);   
    size_t read_len = read(sock, buf.data(), buf.size() - 1);
    buf[read_len] = '\n';
}

Response Client::getResponce() {
    return {buf.data()};
}
