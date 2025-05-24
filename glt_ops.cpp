#include "glt_ops.h"
#include <git2.h>
#include <sstream>
#include <wx/wx.h>
#include <ctime>

std::string get_timestamp() {
    time_t now = time(nullptr);
    char buf[100];
    ctime_s(buf, sizeof(buf), &now);
    buf[strcspn(buf, "\n")] = '\0';
    return std::string(buf);
}

// Custom dialog class for password entry
class PasswordDialog : public wxDialog {
public:
    PasswordDialog(wxWindow* parent, const wxString& message, const wxString& caption)
        : wxDialog(parent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        sizer->Add(new wxStaticText(this, wxID_ANY, message), 0, wxALL, 10);

        passwordCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
        sizer->Add(passwordCtrl, 0, wxALL | wxEXPAND, 10);

        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 5);
        buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALL, 5);
        sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

        SetSizerAndFit(sizer);
    }

    wxString GetValue() const {
        return passwordCtrl->GetValue();
    }

private:
    wxTextCtrl* passwordCtrl;
};

static int credential_cb(git_credential** out, const char* url, const char* username_from_url,
    unsigned int allowed_types, void* payload) {
    if (allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
        wxTextEntryDialog usernameDlg(nullptr, wxString("Enter username for ") + url, "Username", wxString(), wxOK | wxCANCEL);
        if (usernameDlg.ShowModal() != wxID_OK) {
            return GIT_EUSER;
        }
        std::string username = usernameDlg.GetValue().ToStdString();

        // Use custom dialog for password
        PasswordDialog passwordDlg(nullptr, wxString("Enter password/token for ") + url, "Password");
        int result = passwordDlg.ShowModal();
        if (result != wxID_OK) {
            wxLogMessage("Password dialog returned: %d", result);
            return GIT_EUSER;
        }
        std::string password = passwordDlg.GetValue().ToStdString();

        return git_credential_userpass_plaintext_new(out, username.c_str(), password.c_str());
    }
    return GIT_EUSER;
}

GitOps::GitOps(const std::string& repo_path, const std::string& remote_url, wxTextCtrl* log_ctrl)
    : repo(nullptr), remote_url(remote_url), commit_count(0), upstream_set(false), logCtrl(log_ctrl) {
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
        logCtrl->AppendText("[" + get_timestamp() + "] Detached HEAD detected, creating branch 'main'\n");
        git_commit* head_commit = nullptr;
        if (git_revparse_single((git_object**)&head_commit, repo, "HEAD") == 0) {
            git_reference* branch_ref = nullptr;
            git_branch_create(&branch_ref, repo, "main", head_commit, 0);
            git_commit_free(head_commit);
            git_reference_free(branch_ref);
        }
    }

    if (!detect_current_branch(current_branch, error_msg)) {
        logCtrl->AppendText("[" + get_timestamp() + "] Failed to detect current branch: " + error_msg + "\n");
        current_branch = "main";
    }
    else {
        logCtrl->AppendText("[" + get_timestamp() + "] Detected current branch: " + current_branch + "\n");
    }
}

GitOps::~GitOps() {
    git_repository_free(repo);
    git_libgit2_shutdown();
}

bool GitOps::detect_current_branch(std::string& branch_name, std::string& error_msg) {
    git_reference* head = nullptr;
    int error = git_repository_head(&head, repo);
    if (error == GIT_ENOTFOUND) {
        error_msg = "No HEAD found - repository might be empty";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        return false;
    }
    else if (error == GIT_EUNBORNBRANCH) {
        error_msg = "HEAD points to an unborn branch - creating initial branch 'main'";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        branch_name = "main";
        return true;
    }
    else if (git_repository_head_detached(repo) == 1) {
        error_msg = "Repository is in a detached HEAD state - cannot determine branch";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        return false;
    }
    else if (error != 0) {
        const git_error* e = git_error_last();
        error_msg = "Error getting HEAD: " + std::string(e ? e->message : "unknown error");
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        return false;
    }

    const char* branch = git_reference_shorthand(head);
    if (!branch) {
        error_msg = "Error getting branch name: shorthand is null";
        git_reference_free(head);
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        return false;
    }

    branch_name = std::string(branch);
    git_reference_free(head);
    logCtrl->AppendText("[" + get_timestamp() + "] Branch shorthand: " + branch_name + "\n");
    return true;
}

bool GitOps::setup_remote(std::string& error_msg) {
    git_remote* remote = nullptr;
    int error = git_remote_lookup(&remote, repo, "origin");

    if (error == GIT_ENOTFOUND) {
        logCtrl->AppendText("[" + get_timestamp() + "] Remote 'origin' not found, creating it with URL: " + remote_url + "\n");
        error = git_remote_create(&remote, repo, "origin", remote_url.c_str());
        if (error != 0) {
            const git_error* e = git_error_last();
            error_msg = "Error creating remote: " + std::string(e ? e->message : "unknown error");
            return false;
        }
    }
    else if (error != 0) {
        const git_error* e = git_error_last();
        error_msg = "Error looking up remote: " + std::string(e ? e->message : "unknown error");
        return false;
    }
    else {
        const char* url = git_remote_url(remote);
        logCtrl->AppendText("[" + get_timestamp() + "] Remote 'origin' found with URL: " + std::string(url) + "\n");
        if (url && remote_url != url) {
            logCtrl->AppendText("[" + get_timestamp() + "] Remote URL mismatch, updating to: " + remote_url + "\n");
            git_remote_delete(repo, "origin");
            error = git_remote_create(&remote, repo, "origin", remote_url.c_str());
            if (error != 0) {
                const git_error* e = git_error_last();
                error_msg = "Error updating remote: " + std::string(e ? e->message : "unknown error");
                return false;
            }
        }
    }

    git_remote_free(remote);
    return true;
}

bool GitOps::set_upstream(std::string& error_msg) {
    git_reference* branch_ref = nullptr;
    int error = git_branch_lookup(&branch_ref, repo, current_branch.c_str(), GIT_BRANCH_LOCAL);
    if (error == GIT_ENOTFOUND) {
        logCtrl->AppendText("[" + get_timestamp() + "] Local branch '" + current_branch + "' not found, attempting to create it\n");
        git_commit* head_commit = nullptr;
        error = git_revparse_single((git_object**)&head_commit, repo, "HEAD");
        if (error != 0) {
            const git_error* e = git_error_last();
            error_msg = "Error looking up HEAD for branch creation: " + std::string(e ? e->message : "unknown error");
            logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
            return false;
        }

        error = git_branch_create(&branch_ref, repo, current_branch.c_str(), head_commit, 0);
        git_commit_free(head_commit);
        if (error != 0) {
            const git_error* e = git_error_last();
            error_msg = "Error creating branch '" + current_branch + "': " + std::string(e ? e->message : "unknown error");
            logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
            return false;
        }
        logCtrl->AppendText("[" + get_timestamp() + "] Created local branch '" + current_branch + "'\n");
    }
    else if (error != 0) {
        const git_error* e = git_error_last();
        error_msg = "Error looking up local branch '" + current_branch + "': " + std::string(e ? e->message : "unknown error");
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        return false;
    }

    std::string upstream = "origin/" + current_branch;
    error = git_branch_set_upstream(branch_ref, upstream.c_str());
    if (error != 0) {
        const git_error* e = git_error_last();
        error_msg = "Error setting upstream to " + upstream + ": " + std::string(e ? e->message : "unknown error");
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");

        git_remote* remote = nullptr;
        if (git_remote_lookup(&remote, repo, "origin") == 0) {
            git_push_options push_opts;
            if (git_push_init_options(&push_opts, GIT_PUSH_OPTIONS_VERSION) == 0) {
                push_opts.callbacks.credentials = credential_cb;
                push_opts.callbacks.payload = nullptr;

                std::string refspec = "refs/heads/" + current_branch + ":refs/heads/" + current_branch;
                const char* refs[] = { refspec.c_str() };
                git_strarray refspecs = { (char**)refs, 1 };
                error = git_remote_push(remote, &refspecs, &push_opts);
                if (error == 0) {
                    logCtrl->AppendText("[" + get_timestamp() + "] Pushed to create remote branch " + upstream + "\n");
                    error = git_branch_set_upstream(branch_ref, upstream.c_str());
                    if (error == 0) {
                        logCtrl->AppendText("[" + get_timestamp() + "] Successfully set upstream to " + upstream + "\n");
                        git_remote_free(remote);
                        git_reference_free(branch_ref);
                        return true;
                    }
                }
                const git_error* e2 = git_error_last();
                error_msg = "Error pushing to create upstream: " + std::string(e2 ? e2->message : "unknown error");
                logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
            }
            git_remote_free(remote);
        }
        git_reference_free(branch_ref);
        return false;
    }

    logCtrl->AppendText("[" + get_timestamp() + "] Successfully set upstream to " + upstream + "\n");
    git_reference_free(branch_ref);
    return true;
}

std::string GitOps::get_git_logs(int max_commits) {
    std::stringstream log_stream;
    git_revwalk* walker = nullptr;
    git_commit* commit = nullptr;
    git_oid oid;

    if (git_revwalk_new(&walker, repo) != 0) {
        return "Error: Failed to create revwalk\n";
    }

    git_revwalk_sorting(walker, GIT_SORT_TIME);
    if (git_revwalk_push_head(walker) != 0) {
        git_revwalk_free(walker);
        return "Error: No commits found\n";
    }

    int count = 0;
    while (count < max_commits && git_revwalk_next(&oid, walker) == 0) {
        if (git_commit_lookup(&commit, repo, &oid) != 0) {
            continue;
        }

        const git_signature* author = git_commit_author(commit);
        const char* message = git_commit_message(commit);
        git_time_t time = git_commit_time(commit);

        char time_buf[100];
        ctime_s(time_buf, sizeof(time_buf), &time);
        time_buf[strcspn(time_buf, "\n")] = '\0';

        log_stream << "Commit " << git_oid_tostr_s(&oid) << "\n"
            << "Author: " << (author ? author->name : "Unknown") << " <"
            << (author ? author->email : "unknown") << ">\n"
            << "Date: " << time_buf << "\n"
            << "Message: " << (message ? message : "No message") << "\n\n";

        git_commit_free(commit);
        count++;
    }

    git_revwalk_free(walker);
    return log_stream.str();
}

bool GitOps::commit_and_push(const std::string& message, std::string& error_msg) {
    git_signature* sig = nullptr;
    git_index* index = nullptr;
    git_oid tree_id, commit_id;
    git_tree* tree = nullptr;
    git_remote* remote = nullptr;

    logCtrl->AppendText("[" + get_timestamp() + "] Starting commit_and_push: " + message + "\n");

    if (git_signature_now(&sig, "Lazy Git", "lazy@git.com") != 0) {
        error_msg = "Error creating signature";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        return false;
    }

    if (git_repository_index(&index, repo) != 0) {
        error_msg = "Error getting index";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_signature_free(sig);
        return false;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Git index loaded\n");

    git_strarray paths = { nullptr, 0 };
    if (git_index_add_all(index, &paths, 0, nullptr, nullptr) != 0) {
        error_msg = "Error adding files to index";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Files added to index\n");

    if (git_index_write(index) != 0) {
        error_msg = "Error writing index";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Index written\n");

    if (git_index_write_tree(&tree_id, index) != 0) {
        error_msg = "Error writing tree - likely no changes to commit";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Tree written\n");

    if (git_tree_lookup(&tree, repo, &tree_id) != 0) {
        error_msg = "Error looking up tree";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }

    git_object* parent = nullptr;
    git_reference* ref = nullptr;
    int error = git_revparse_single(&parent, repo, "HEAD");
    if (error == GIT_ENOTFOUND) {
        logCtrl->AppendText("[" + get_timestamp() + "] No previous commits found, creating initial commit\n");
        parent = nullptr;
    }
    else if (error != 0) {
        error_msg = "Error looking up HEAD";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_tree_free(tree);
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }

    commit_count++;
    std::string commit_message = "commit " + std::to_string(commit_count);
    if (git_commit_create(&commit_id, repo, "HEAD", sig, sig, nullptr, commit_message.c_str(), tree,
        parent ? 1 : 0, parent ? (const git_commit**)&parent : nullptr) != 0) {
        error_msg = "Error creating commit";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        if (parent) git_object_free(parent);
        git_tree_free(tree);
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Commit created: " + commit_message + "\n");

    if (git_remote_lookup(&remote, repo, "origin") != 0) {
        error_msg = "Error looking up remote";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        if (parent) git_object_free(parent);
        git_tree_free(tree);
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }

    if (!upstream_set) {
        if (!set_upstream(error_msg)) {
            git_remote_free(remote);
            if (parent) git_object_free(parent);
            git_tree_free(tree);
            git_index_free(index);
            git_signature_free(sig);
            return false;
        }
        upstream_set = true;
    }
    else {
        logCtrl->AppendText("[" + get_timestamp() + "] Upstream already set, skipping set_upstream\n");
    }

    git_push_options push_opts;
    if (git_push_init_options(&push_opts, GIT_PUSH_OPTIONS_VERSION) != 0) {
        error_msg = "Error initializing push options";
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_remote_free(remote);
        if (parent) git_object_free(parent);
        git_tree_free(tree);
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }

    push_opts.callbacks.credentials = credential_cb;
    push_opts.callbacks.payload = nullptr;

    std::string refspec = "refs/heads/" + current_branch + ":refs/heads/" + current_branch;
    const char* refs[] = { refspec.c_str() };
    git_strarray refspecs = { (char**)refs, 1 };
    if (git_remote_push(remote, &refspecs, &push_opts) != 0) {
        const git_error* e = git_error_last();
        error_msg = "Error pushing: " + std::string(e ? e->message : "unknown error");
        logCtrl->AppendText("[" + get_timestamp() + "] " + error_msg + "\n");
        git_remote_free(remote);
        if (parent) git_object_free(parent);
        git_tree_free(tree);
        git_index_free(index);
        git_signature_free(sig);
        return false;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Push successful\n");

    git_remote_free(remote);
    if (parent) git_object_free(parent);
    git_tree_free(tree);
    git_index_free(index);
    git_signature_free(sig);

    return true;
}