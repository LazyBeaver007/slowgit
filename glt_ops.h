#ifndef LAZY_GIT_GIT_OPS_HPP
#define LAZY_GIT_GIT_OPS_HPP

#include <string>
#include <git2.h>
#include "config.h" // Include the new project config header

#pragma warning(push)
#pragma warning(disable: 4996)
#include <wx/wx.h>
#include <wx/textctrl.h>
#pragma warning(pop)

class GitOps {
public:
    // The constructor now accepts a reference to the project's configuration object.
    GitOps(const std::string& repo_path, ProjectConfig& config, wxTextCtrl* log_ctrl);
    ~GitOps();
    bool commit_and_push(const std::string& message, std::string& error_msg);
    bool setup_remote(std::string& error_msg);
    std::string get_git_logs(int max_commits = 5);

private:
    bool set_upstream(std::string& error_msg);
    bool detect_current_branch(std::string& branch_name, std::string& error_msg);

    git_repository* repo;
    std::string repo_path;      // Keep the repo path for saving the config.
    ProjectConfig& projectConfig; // A reference to the active project's config.
    std::string current_branch;
    bool upstream_set;
    wxTextCtrl* logCtrl;
};

#endif // LAZY_GIT_GIT_OPS_HPP
