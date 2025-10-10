#pragma once
#include <string>
#include "config.hpp"

struct Args {
    bool help{};
    bool version{};
    bool server{};
    int port{};
    std::string message;
    std::string server_ip;
};

Args parse_args(int argc, char** argv, const Config& cfg);
void print_help();
