#pragma once
#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP

#include <string>
#include <nlohmann/json.hpp>

// Defines the configuration that is specific to each project.
struct ProjectConfig
{
    // Configuration fields for each project.
    std::string remote_url;
    std::string auth_token; // The GitHub OAuth token
    static int commit_count;
    int push_count = 0;

    // A helper function to get the standardized path for the config file.
    static std::string getConfigPath(const std::string& project_path);

    // Saves this configuration to the project's .lazygit directory.
    void save(const std::string& project_path);

    // Loads a configuration from the project's .lazygit directory.
    static ProjectConfig load(const std::string& project_path);
};

#endif // !PROJECT_CONFIG_HPP
