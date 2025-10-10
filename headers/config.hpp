#pragma once
#include <string>
#include <toml++/toml.h>

/**
 * @brief Holds configuration values loaded from a TOML file.
 * 
 * Fields are populated from the user's configuration file if it exists,
 * or from defaults if the file is missing.
 */
struct Config {
    int port;              ///< Default port (default: 50001)
    std::string message;   ///< Default message (default: "Hello from client")
};

/**
 * @brief Loads configuration from the standard platform-specific config path.
 * 
 * On Linux/macOS, the path is ~/.config/{pkgname}/config.toml.
 * On Windows, the path is %APPDATA%\{pkgname}\config.toml.
 * 
 * If the file does not exist, a new one is created with default values.
 * 
 * @param pkgname Name of the package/application, used to determine config path.
 * @return Config Populated configuration values.
 */
Config load_config(const std::string& pkgname);

