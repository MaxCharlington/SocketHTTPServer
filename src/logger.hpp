#pragma once

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

// Logger class helper to log nowhere
struct void_ostream {};

void_ostream& operator<<(void_ostream& l, auto) {
    return l;
}


template <typename Console = std::ostream&, typename Stream = void_ostream, size_t BUFFSIZE = 1024>
class Logger {
    Console console;
    Stream stream;

public:
    Logger(Console console_ = std::clog, Stream stream_ = void_ostream{}) : console{console_}, stream{stream_} {}
    Logger(std::ofstream& stream_) : console{std::clog}, stream{stream_} {}

    template <typename ...T>
    void log(T... args) {
        (console << ... << args);
        (stream << ... << args);
        console << "\n\n";
        stream << "\n\n";
    }

    template <typename ...T>
    void log_format(const char* format, T... args) {
        static char buf[BUFFSIZE];
        snprintf(buf, BUFFSIZE, format, args...);
        log(buf);
    }

    // Outputs every step of chaining with newline
    Logger& operator<<(auto arg) {
        log(arg);
        return *this;
    }
};
