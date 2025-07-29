#include "glt_ops.h"
#include "utils.h"
#include <git2.h>
#include <sstream>
#include <wx/wx.h>
#include <wx/log.h>

// The payload for the credential callback is now a pointer to the ProjectConfig.
static int credential_cb(git_credential** out, const char* url, const char* username_from_url,
    unsigned int allowed_types, void* payload) {

    // We cast the payload back to the ProjectConfig pointer.
    ProjectConfig* config = static_cast<ProjectConfig*>(payload);
    if (!config) return GIT_EUSER;

    // We use the token from the project's specific config.
    if ((allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) && !config->auth_token.empty()) {
        return git_credential_userpass_plaintext_new(out, "oauth2", config->auth_token.c_str());
    }
    return GIT_EUSER;
}

// Updated constructor to accept ProjectConfig.
GitOps::GitOps(const std::string& repo_path, ProjectConfig& config, wxTextCtrl* log_ctrl)
    : repo(nullptr), repo_path(repo_path), projectConfig(config), commit_count(0), upstream_set(false), logCtrl(log_ctrl) {
    git_libgit2_init();
    if (git_repository_open(&repo, repo_path.c_str()) != 0) {
        const git_error* e = git_error_last();
        throw std::runtime_error("Failed to open Git repository: " + std::string(e ? e->message : "unknown error"));
    }
    std::string error_msg;
    if (!setup_remote(error_msg)) {
        logCtrl->AppendText("[" + get_timestamp() + "] Setup remote failed: " + error_msg + "\n");
    }

    if (git_repository_head_detached(repo) == 1) {
        // ... (rest of constructor is largely the same)
    }
    // ...
}

GitOps::~GitOps() {
    git_repository_free(repo);
    git_libgit2_shutdown();
}

bool GitOps::setup_remote(std::string& error_msg) {
    // This function now uses `projectConfig.remote_url`
    git_remote* remote = nullptr;
    int error = git_remote_lookup(&remote, repo, "origin");

    if (error == GIT_ENOTFOUND) {
        logCtrl->AppendText("[" + get_timestamp() + "] Remote 'origin' not found, creating it with URL: " + projectConfig.remote_url + "\n");
        error = git_remote_create(&remote, repo, "origin", projectConfig.remote_url.c_str());
        // ...
    }
    else {
        const char* url = git_remote_url(remote);
        if (url && projectConfig.remote_url != url) {
            logCtrl->AppendText("[" + get_timestamp() + "] Remote URL mismatch, updating to: " + projectConfig.remote_url + "\n");
            git_remote_delete(repo, "origin");
            error = git_remote_create(&remote, repo, "origin", projectConfig.remote_url.c_str());
            // ...
        }
    }
    // ...
    return true;
}

// ... (other methods like get_git_logs, detect_current_branch, set_upstream remain mostly the same) ...

bool GitOps::commit_and_push(const std::string& message, std::string& error_msg) {
    git_signature* sig = nullptr;
    git_index* index = nullptr;
    git_oid tree_id, commit_id;
    git_tree* tree = nullptr;
    git_remote* remote = nullptr;

    // ... (signature, index, add, write, tree logic is the same) ...

    // After git_tree_lookup succeeds:
    git_object* parent = nullptr;
    // ... (parent lookup logic is the same) ...

    // The commit message now uses the count from the config.
    std::string commit_message = "commit " + std::to_string(projectConfig.commit_count + 1);

    if (git_commit_create(&commit_id, repo, "HEAD", sig, sig, nullptr, commit_message.c_str(), tree,
        parent ? 1 : 0, parent ? (const git_commit**)&parent : nullptr) != 0) {
        // ... (error handling) ...
        return false;
    }

    projectConfig.commit_count++; // Increment commit count
    logCtrl->AppendText("[" + get_timestamp() + "] Commit created: " + commit_message + "\n");

    // ... (remote lookup logic is the same) ...

    // ... (set_upstream logic is the same) ...

    git_push_options push_opts;
    git_push_init_options(&push_opts, GIT_PUSH_OPTIONS_VERSION);

    // IMPORTANT: Pass the address of the projectConfig object as the payload.
    push_opts.callbacks.credentials = credential_cb;
    push_opts.callbacks.payload = &projectConfig;

    std::string refspec = "refs/heads/" + current_branch + ":refs/heads/" + current_branch;
    const char* refs[] = { refspec.c_str() };
    git_strarray refspecs = { (char**)refs, 1 };

    if (git_remote_push(remote, &refspecs, &push_opts) != 0) {
        const git_error* e = git_error_last();
        error_msg = "Error pushing: " + std::string(e ? e->message : "unknown error");
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        // Don't save on failure
        // ... (cleanup) ...
        return false;
    }

    logCtrl->AppendText("[" + get_timestamp() + "] Push successful\n");
    projectConfig.push_count++; // Increment push count
    projectConfig.save(repo_path); // Save the config with new counts.
    logCtrl->AppendText("[" + get_timestamp() + "] Project config saved with updated counts.\n");

    // ... (cleanup code) ...

    return true;
}
