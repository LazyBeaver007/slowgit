#ifndef LAZY_GIT_WATCHER_HPP
#define LAZY_GIT_WATCHER_HPP

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(pop)
#include <string>
#include <efsw/efsw.hpp>
#include "glt_ops.h"  // Corrected from "glt_ops.h" to "git_ops.hpp"
#include <wx/textctrl.h>

class FileWatcher : public efsw::FileWatchListener {
public:
    FileWatcher(const std::string& path, GitOps& git_ops, bool commit_after_save, wxTextCtrl* log_ctrl);
    void handleFileAction(efsw::WatchID watchid, const std::string& dir,
        const std::string& filename, efsw::Action action,
        std::string oldFilename) override;
    void stop();

private:
    GitOps& git_ops;
    bool commit_after_save;
    efsw::FileWatcher* watcher;
    efsw::WatchID watch_id;
    wxTextCtrl* logCtrl;  // Added to match the implementation in watcher.cpp
};

#endif // LAZY_GIT_WATCHER_HPP