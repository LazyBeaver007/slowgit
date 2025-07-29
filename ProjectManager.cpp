#include "ProjectManager.h"
#include <fstream>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/dir.h>

ProjectManager::ProjectManager() {
    // We'll store the central projects.json in the user's application data directory.
    wxString userDataDir = wxStandardPaths::Get().GetUserDataDir();

    // wxWidgets makes it easy to ensure this directory exists.
    if (!wxDirExists(userDataDir)) {
        wxMkdir(userDataDir);
    }

    // Construct the full path to the config file.
    wxFileName configFile(userDataDir, "projects.json");
    configFilePath = configFile.GetFullPath().ToStdString();

    // Load projects on startup.
    loadProjects();
}

void ProjectManager::loadProjects() {
    projects.clear();
    std::ifstream file(configFilePath);
    if (file.is_open()) {
        json j;
        try {
            file >> j;
            
            if (j.contains("projects")) {
                for (const auto& item : j["projects"]) {
                   
                    projects.push_back({
                        item.value("name", ""),
                        item.value("path", "")
                        });
                }
            }
        }
        catch (...) {
            
        }
    }
}

void ProjectManager::saveProjects() {
    json j;
    json projectArray = json::array();
    for (const auto& p : projects) {
        json projectObj;
        projectObj["name"] = p.name;
        projectObj["path"] = p.path;
        projectArray.push_back(projectObj);
    }
    j["projects"] = projectArray;

    std::ofstream file(configFilePath);
    if (file.is_open()) {
        file << j.dump(4); 
    }
}

void ProjectManager::addProject(const ProjectInfo& project) {
    projects.push_back(project);
    saveProjects();
}

const std::vector<ProjectInfo>& ProjectManager::getProjects() const {
    return projects;
}

std::string ProjectManager::getConfigPath() {
    return configFilePath;
}
