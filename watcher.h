#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <string>
#include <thread>
#include <atomic>
#include <filesystem>
#include <wx/wx.h>
#include <wx/textctrl.h>
#include "glt_ops.h"

class FileWatcher {
public:
    FileWatcher(const std::string& path, bool commit_after_save, GitOps& git_ops, wxTextCtrl* log_ctrl);
    ~FileWatcher();
    void startWatching();
    void stopWatching();

private:
    void watch();
    std::string path;
    bool commit_after_save;
    GitOps& git_ops;
    wxTextCtrl* logCtrl;
    std::thread watcher_thread;
    std::atomic<bool> running;
    std::filesystem::file_time_type last_modified;
};

#endif // FILE_WATCHER_HPP