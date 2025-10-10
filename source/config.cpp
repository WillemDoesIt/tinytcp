#include "config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

fs::path get_config_file(const std::string& pkg) {
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (!appdata) throw std::runtime_error("APPDATA not set");
    return fs::path(appdata) / pkg / "config.toml";
#else
    const char* home = std::getenv("HOME");
    if (!home) throw std::runtime_error("HOME not set");
    return fs::path(home) / ".config" / pkg / "config.toml";
#endif
}

Config load_config(const std::string& pkgname) {
    fs::path configFile = get_config_file(pkgname);
    fs::path configDir = configFile.parent_path();
    Config cfg;

    if (!fs::exists(configFile)) {
        fs::create_directories(configDir);
        std::ofstream out(configFile);
        out << "[defaults]\nport = 50001\nmessage = \"Hello from client\"\n";
        std::cout << "Created default config at " << configFile << "\n";
    }

    // Parse file
    auto tbl = toml::parse_file(configFile.string());
    auto defaults = tbl["defaults"];
    cfg.port = defaults["port"].value_or(50001);
    cfg.message = defaults["message"].value_or("Hello from client");

    return cfg;
}

