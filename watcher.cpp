#include "watcher.h"
#include "utils.h"  // Added for get_timestamp
#include <filesystem>
#include <chrono>
#include <wx/wx.h>
#include <curl.h>

FileWatcher::FileWatcher(const std::string& path, bool commit_after_save, GitOps& git_ops, wxTextCtrl* log_ctrl)
    : path(path), commit_after_save(commit_after_save), git_ops(git_ops), logCtrl(log_ctrl), running(false) {
    if (std::filesystem::exists(path)) {
        last_modified = std::filesystem::last_write_time(path);
    }
}

FileWatcher::~FileWatcher() {
    stopWatching();
}

void FileWatcher::startWatching() {
    if (!running) {
        running = true;
        watcher_thread = std::thread(&FileWatcher::watch, this);
    }
}

void FileWatcher::stopWatching() {
    running = false;
    if (watcher_thread.joinable()) {
        watcher_thread.join();
    }
}

void FileWatcher::watch() {
    while (running) {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (entry.path().filename() == ".git") continue;

                auto current_modified = entry.last_write_time();
                if (current_modified > last_modified) {
                    std::string filepath = entry.path().string();
                    logCtrl->AppendText("[" + get_timestamp() + "] File Added: " + filepath + "\n");

                    if (commit_after_save) {
                        std::string message = "Auto-commit: " + entry.path().filename().string();
                        std::string error_msg;
                        if (git_ops.commit_and_push(message, error_msg)) {
                            logCtrl->AppendText("[" + get_timestamp() + "] Git Logs:\n" + git_ops.get_git_logs() + "\n");
                        }
                        else {
                            logCtrl->AppendText("[" + get_timestamp() + "] Commit/Push failed: " + error_msg + "\n");
                        }
                    }

                    last_modified = current_modified;
                }
            }
        }
        catch (const std::exception& e) {
            logCtrl->AppendText("[" + get_timestamp() + "] Error watching directory: " + std::string(e.what()) + "\n");
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}