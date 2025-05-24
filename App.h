#ifndef LAZY_GIT_APP_HPP
#define LAZY_GIT_APP_HPP

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(pop)

#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include "config.h"
#include "glt_ops.h"
#include "watcher.h"

class ConfigDialog : public wxDialog {
public:
    ConfigDialog(wxWindow* parent, LazyGitConfig& config);
    LazyGitConfig GetConfig();
    wxCheckBox* saveCheck;
    wxTextCtrl* intervalCtrl;

private:
    LazyGitConfig config;
};

class LazyGitFrame : public wxFrame {
public:
    LazyGitFrame(const wxString& title);

public:
    void OnInitFolder(wxCommandEvent& event);
    void OnConfigure(wxCommandEvent& event);
    void OnToggleTracking(wxCommandEvent& event);

    wxTextCtrl* logCtrl;
    wxButton* initButton;
    wxButton* configButton;
    wxButton* trackButton;
    bool isTracking;

    LazyGitConfig config;
    std::unique_ptr<FileWatcher> fileWatcher;
    GitOps* gitOps;

    wxDECLARE_EVENT_TABLE();
};

class LazyGitApp : public wxApp {
public:
    virtual bool OnInit() override;
};

#endif // LAZY_GIT_APP_HPP