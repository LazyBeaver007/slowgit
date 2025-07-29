#pragma once
#ifndef PROJECT_MANAGER_HPP
#define PROJECT_MANAGER_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// A simple struct to hold basic information about a project.
struct ProjectInfo {
    std::string name;
    std::string path;
};

// Manages loading, saving, and accessing the list of all known projects.
class ProjectManager {
public:
    // Constructor initializes the path to projects.json and loads existing projects.
    ProjectManager();

    // Reloads the project list from the JSON file.
    void loadProjects();

    // Saves the current list of projects back to the JSON file.
    void saveProjects();

    // Adds a new project to the list and saves the file.
    void addProject(const ProjectInfo& project);

    // Returns a constant reference to the vector of projects.
    const std::vector<ProjectInfo>& getProjects() const;

    // Gets the full path to the central projects.json config file.
    std::string getConfigPath();

private:
    std::string configFilePath;
    std::vector<ProjectInfo> projects;
    using json = nlohmann::json; // Alias for convenience
};

#endif // PROJECT_MANAGER_HPP
