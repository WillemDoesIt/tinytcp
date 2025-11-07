#include <fstream>
#include <filesystem>

std::filesystem::path get_config_path() {
#ifdef _WIN32
    const char* APPDATA = std::getenv("APPDATA");
    std::filesystem::path base = APPDATA ? APPDATA : ".";
    base /= "tinytcp";
#else
    const char* HOME = std::getenv("HOME");
    std::filesystem::path base = HOME ? HOME : ".";
    base /= ".config/tinytcp";
#endif
    std::filesystem::create_directories(base);
    return base / "config.toml";
}

struct Config {
    int port = 49153;
    std::string message = "Hello from client!";
};

Config load_config() {
    Config cfg;
    auto path = get_config_path();

    if (!std::filesystem::exists(path)) {
        std::ofstream out(path);
        out << "port = " << cfg.port << "\nmessage = \"" << cfg.message <<"\"\n";
        std::cout << "Created default config at " << path << "\n";
        return cfg;
    }
    try {
        auto tbl = toml::parse_file(path.string());
        if (auto p = tbl["port"].value<int>()) { 
            cfg.port = *p;
        }
        if (auto m = tbl["message"].value<std::string>()) {
            cfg.message = *m;
        }
    } catch (const toml::parse_error& ERR) {
        std::cerr << "Error parsing " << path << ": " << ERR.description() << "\n";
    }
    return cfg;
}
