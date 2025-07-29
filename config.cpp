#include "config.h"
#include <fstream>
#include <filesystem> // Using C++17 filesystem for directory operations.

using json = nlohmann::json;

// The config file for a project will be stored in a ".lazygit" subfolder.
std::string ProjectConfig::getConfigPath(const std::string& project_path) {
    return project_path + "/.lazygit/config.json";
}

void ProjectConfig::save(const std::string& project_path)
{
    std::string config_path_str = getConfigPath(project_path);
    std::filesystem::path config_path(config_path_str);

    // Get the parent directory (e.g., /path/to/project/.lazygit)
    std::filesystem::path config_dir = config_path.parent_path();

    // Create the .lazygit directory if it doesn't already exist.
    if (!std::filesystem::exists(config_dir)) {
        std::filesystem::create_directory(config_dir);
    }

    json j;
    j["remote_url"] = remote_url;
    j["auth_token"] = auth_token;
    j["commit_count"] = commit_count;
    j["push_count"] = push_count;

    std::ofstream file(config_path_str);
    if (file.is_open()) {
        file << j.dump(4);
    }
}

ProjectConfig ProjectConfig::load(const std::string& project_path)
{
    ProjectConfig config;
    std::string config_path = getConfigPath(project_path);

    if (std::filesystem::exists(config_path)) {
        try {
            std::ifstream file(config_path);
            if (file.is_open()) {
                json j;
                file >> j;
                // Use .value() for safe parsing. If a key doesn't exist, it uses the provided default.
                config.remote_url = j.value("remote_url", "");
                config.auth_token = j.value("auth_token", "");
                config.commit_count = j.value("commit_count", 0);
                config.push_count = j.value("push_count", 0);
            }
        }
        catch (...)
        {
            // If the file is corrupt or unreadable, we'll just return a default config object.
        }
    }
    return config;
}
