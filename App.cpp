#include "App.h"
#include <wx/sizer.h>
#include "config.h"
#include <wx/dirdlg.h>
#include <wx/textdlg.h>
#include <filesystem>
#include "glt_ops.h"
#include "watcher.h"
#include <io.h>  
#include <fcntl.h>  
#include <share.h>  
#include <nlohmann/json.hpp>


#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(pop)

wxIMPLEMENT_APP(LazyGitApp);

bool LazyGitApp::OnInit() {
    LazyGitFrame* frame = new LazyGitFrame("Lazy Git");
    frame->Show(true);
    return true;
}

wxBEGIN_EVENT_TABLE(LazyGitFrame, wxFrame)
EVT_BUTTON(10001, LazyGitFrame::OnInitFolder)
EVT_BUTTON(10002, LazyGitFrame::OnConfigure)
EVT_BUTTON(10003, LazyGitFrame::OnToggleTracking)
wxEND_EVENT_TABLE()

LazyGitFrame::LazyGitFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
    isTracking(false), fileWatcher(nullptr), gitOps(nullptr) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    initButton = new wxButton(panel, 10001, "Initialize Folder");
    configButton = new wxButton(panel, 10002, "Configure Preferences");
    trackButton = new wxButton(panel, 10003, "Start Tracking");

    logCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY);

    sizer->Add(initButton, 0, wxALL | wxEXPAND, 5);
    sizer->Add(configButton, 0, wxALL | wxEXPAND, 5);
    sizer->Add(trackButton, 0, wxALL | wxEXPAND, 5);
    sizer->Add(logCtrl, 1, wxALL | wxEXPAND, 5);

    panel->SetSizer(sizer);
    Centre();
}

void LazyGitFrame::OnInitFolder(wxCommandEvent& event) {
    logCtrl->AppendText("Initialize Folder clicked\n");
    wxDirDialog dirDlg(this, "Select folder to initialize", "", wxDD_DEFAULT_STYLE);
    if (dirDlg.ShowModal() != wxID_OK) return;

    std::string folder_path = dirDlg.GetPath().ToStdString();
    if (!std::filesystem::exists(folder_path + "/.git")) {
        logCtrl->AppendText("Error: No .git folder found in " + folder_path + "\n");
        return;
    }

    wxTextEntryDialog urlDlg(this, "Enter remote repository URL", "Remote URL");
    if (urlDlg.ShowModal() != wxID_OK) return;

    config.folder_path = folder_path;
    config.remote_url = urlDlg.GetValue().ToStdString();
    config.commit_after_save = true;
    config.commit_interval_minutes = 0;

    // Save config using _wsopen_s instead of std::ofstream
    std::string config_path = folder_path + "/lazy-git.json";
    nlohmann::json j;
    j["folder_path"] = config.folder_path;
    j["remote_url"] = config.remote_url;
    j["commit_after_save"] = config.commit_after_save;
    j["commit_interval_minutes"] = config.commit_interval_minutes;

    std::string json_str = j.dump(4);
    int fd;
    errno_t err = _wsopen_s(&fd, wxString(config_path).ToStdWstring().c_str(),
        _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY,
        _SH_DENYNO, _S_IREAD | _S_IWRITE);
    if (err != 0) {
        logCtrl->AppendText("Error: Could not open file for writing: " + config_path + "\n");
        return;
    }

    if (_write(fd, json_str.c_str(), json_str.length()) == -1) {
        logCtrl->AppendText("Error: Could not write to file: " + config_path + "\n");
        _close(fd);
        return;
    }

    _close(fd);
    logCtrl->AppendText("Initialized folder: " + folder_path + "\n");
}

ConfigDialog::ConfigDialog(wxWindow* parent, LazyGitConfig& cfg)
    : wxDialog(parent, wxID_ANY, "Configure Preference", wxDefaultPosition, wxSize(300, 200)),
    config(cfg) {
    wxLogMessage("ConfigDialog created with: commit_after_save=%s, commit_interval_minutes=%d",
        config.commit_after_save ? "true" : "false", config.commit_interval_minutes);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    saveCheck = new wxCheckBox(this, wxID_ANY, "Commit after each file save");
    saveCheck->SetValue(config.commit_after_save);
    sizer->Add(saveCheck, 0, wxALL, 10);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Commit every X minutes (0 to disable):"), 0, wxALL, 10);
    intervalCtrl = new wxTextCtrl(this, wxID_ANY, std::to_string(config.commit_interval_minutes));
    sizer->Add(intervalCtrl, 0, wxALL, 10);

    wxButton* okButton = new wxButton(this, wxID_OK, "OK");
    okButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        wxLogMessage("OK button pressed, values: commit_after_save=%s, commit_interval_minutes=%s",
            saveCheck->GetValue() ? "true" : "false", intervalCtrl->GetValue());
        EndModal(wxID_OK);
        });

    sizer->Add(okButton, 0, wxALL | wxALIGN_RIGHT, 10);

    SetSizerAndFit(sizer);

    wxLogMessage("Dialog controls set up with: saveCheck=%s, intervalCtrl=%s",
        saveCheck->GetValue() ? "true" : "false", intervalCtrl->GetValue());
}

LazyGitConfig ConfigDialog::GetConfig() {
    config.commit_after_save = saveCheck->GetValue();

    wxString intervalStr = intervalCtrl->GetValue();
    long interval = 0;
    if (intervalStr.ToLong(&interval)) {
        config.commit_interval_minutes = static_cast<int>(interval);
    }

    return config;
}

void LazyGitFrame::OnConfigure(wxCommandEvent& event) {
    logCtrl->AppendText("Configure Preferences clicked\n");

    wxDirDialog dirDlg(this, "Select your initialized project folder", "", wxDD_DEFAULT_STYLE);
    if (dirDlg.ShowModal() != wxID_OK) {
        logCtrl->AppendText("Configuration cancelled - no folder selected.\n");
        return;
    }

    std::string folder_path = dirDlg.GetPath().ToStdString();
    std::string config_path = folder_path + "/lazy-git.json";

    logCtrl->AppendText("Looking for config at: " + config_path + "\n");

    if (!std::filesystem::exists(config_path)) {
        logCtrl->AppendText("Error: No config file found at " + config_path + "\n");
        return;
    }

    config = LazyGitConfig::load(config_path);

    logCtrl->AppendText("Current config values: commit_after_save=" +
        std::string(config.commit_after_save ? "true" : "false") +
        ", commit_interval_minutes=" + std::to_string(config.commit_interval_minutes) + "\n");

    ConfigDialog dlg(this, config);

    if (dlg.ShowModal() == wxID_OK) {
        bool newCommitAfterSave = dlg.saveCheck->GetValue();

        wxString intervalStr = dlg.intervalCtrl->GetValue();
        int intervalMinutes = 0;
        if (!intervalStr.ToInt(&intervalMinutes)) {
            intervalMinutes = config.commit_interval_minutes;
        }

        logCtrl->AppendText("New config values: commit_after_save=" +
            std::string(newCommitAfterSave ? "true" : "false") +
            ", commit_interval_minutes=" + std::to_string(intervalMinutes) + "\n");

        config.commit_after_save = newCommitAfterSave;
        config.commit_interval_minutes = intervalMinutes;

        try {
            nlohmann::json j;
            j["folder_path"] = config.folder_path;
            j["remote_url"] = config.remote_url;
            j["commit_after_save"] = config.commit_after_save;
            j["commit_interval_minutes"] = config.commit_interval_minutes;

            std::string json_str = j.dump(4);
            int fd;
            errno_t err = _wsopen_s(&fd, wxString(config_path).ToStdWstring().c_str(),
                _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY,
                _SH_DENYNO, _S_IREAD | _S_IWRITE);
            if (err != 0) {
                logCtrl->AppendText("Error: Could not open file for writing: " + config_path + "\n");
                return;
            }

            if (_write(fd, json_str.c_str(), json_str.length()) == -1) {
                logCtrl->AppendText("Error: Could not write to file: " + config_path + "\n");
                _close(fd);
                return;
            }

            _close(fd);
            logCtrl->AppendText("Configuration saved successfully to " + config_path + "\n");
        }
        catch (const std::exception& e) {
            logCtrl->AppendText("Error saving config: " + std::string(e.what()) + "\n");
        }
    }
}

void LazyGitFrame::OnToggleTracking(wxCommandEvent& event) {
    isTracking = !isTracking;
    trackButton->SetLabel(isTracking ? "Stop Tracking" : "Start Tracking");

    std::string config_path = config.folder_path + "/lazy-git.json";
    if (!std::filesystem::exists(config_path)) {
        logCtrl->AppendText("Error: No config file found. Please initialize a folder.\n");
        isTracking = false;
        trackButton->SetLabel("Start Tracking");
        return;
    }

    config = LazyGitConfig::load(config_path);

    if (isTracking) {
        try {
            gitOps = new GitOps(config.folder_path, config.remote_url, logCtrl);
            logCtrl->AppendText("Initial Git Logs:\n" + gitOps->get_git_logs() + "\n");
            fileWatcher = std::make_unique<FileWatcher>(config.folder_path, *gitOps, config.commit_after_save, logCtrl);
            logCtrl->AppendText("Started tracking\n");
        }
        catch (const std::exception& e) {
            logCtrl->AppendText("Error starting tracking: " + std::string(e.what()) + "\n");
            isTracking = false;
            trackButton->SetLabel("Start Tracking");
            gitOps = nullptr;
            fileWatcher.reset();
        }
    }
    else {
        if (fileWatcher) {
            fileWatcher->stop();
            fileWatcher.reset();
        }
        if (gitOps) {
            logCtrl->AppendText("Final Git Logs:\n" + gitOps->get_git_logs() + "\n");
            delete gitOps;
            gitOps = nullptr;
        }
        logCtrl->AppendText("Stopped tracking\n");
    }
}