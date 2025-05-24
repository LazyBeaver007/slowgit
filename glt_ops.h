#ifndef LAZY_GIT_GIT_OPS_HPP
#define LAZY_GIT_GIT_OPS_HPP

#include <string>
#include <git2.h>

#pragma warning(push)
#pragma warning(disable: 4996)

#include <wx/wx.h>
#include <wx/textctrl.h>

#pragma warning(pop)

class GitOps {
public:
    GitOps(const std::string& repo_path, const std::string& remote_url, wxTextCtrl* log_ctrl);
    ~GitOps();
    bool commit_and_push(const std::string& message, std::string& error_msg);
    bool setup_remote(std::string& error_msg);
    std::string get_git_logs(int max_commits = 5);

private:
    bool set_upstream(std::string& error_msg);
    bool detect_current_branch(std::string& branch_name, std::string& error_msg);
    git_repository* repo;
    std::string remote_url;
    std::string current_branch;
    int commit_count;
    bool upstream_set;  // New member to track if upstream has been set
    wxTextCtrl* logCtrl;
};

#endif // LAZY_GIT_GIT_OPS_HPP