#include "watcher.h"
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(pop)

FileWatcher::FileWatcher(const std::string& path, GitOps& git_ops, bool commit_after_save, wxTextCtrl* log_ctrl)
    : git_ops(git_ops), commit_after_save(commit_after_save), watcher(new efsw::FileWatcher), logCtrl(log_ctrl) {
    watch_id = watcher->addWatch(path, this, true);
    watcher->watch();
    logCtrl->AppendText("FileWatcher started for path: " + path + "\n");
}

void FileWatcher::handleFileAction(efsw::WatchID watchid, const std::string& dir,
    const std::string& filename, efsw::Action action,
    std::string oldFilename) {
    if (!commit_after_save) {
        logCtrl->AppendText("File change detected but commit_after_save is false: " + filename + "\n");
        return;
    }

    std::string action_str;
    switch (action) {
    case efsw::Actions::Add: action_str = "Added"; break;
    case efsw::Actions::Modified: action_str = "Modified"; break;
    case efsw::Actions::Delete: action_str = "Deleted"; break;
    case efsw::Actions::Moved: action_str = "Moved"; break;
    default: action_str = "Unknown"; break;
    }

    logCtrl->AppendText("File " + action_str + ": " + dir + "/" + filename + "\n");

    if (action == efsw::Actions::Add || action == efsw::Actions::Modified) {
        std::string error_msg;
        if (git_ops.commit_and_push("Auto-commit: " + filename, error_msg)) {
            logCtrl->AppendText("Successfully committed and pushed: " + filename + "\n");
            logCtrl->AppendText("Git Logs:\n" + git_ops.get_git_logs() + "\n");
        }
        else {
            logCtrl->AppendText("Failed to commit/push: " + filename + " - " + error_msg + "\n");
        }
    }
}

void FileWatcher::stop() {
    if (watcher) {
        watcher->removeWatch(watch_id);
        delete watcher;
        watcher = nullptr;
    }
    logCtrl->AppendText("FileWatcher stopped\n");
}