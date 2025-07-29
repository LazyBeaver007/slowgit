#ifndef LAZY_GIT_APP_HPP
#define LAZY_GIT_APP_HPP

#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include "watcher.h"
#include "glt_ops.h"

class App : public wxApp {
public:
    virtual bool OnInit();
    void OnToggleTracking(wxCommandEvent& event);
    void OnGitHubLogin(wxCommandEvent& event);  // Event handler for GitHub login

private:
    wxFrame* frame;
    wxDirPickerCtrl* dirPicker;
    wxButton* startStopButton;
    wxButton* gitHubLoginButton;  // Button for GitHub login
    wxCheckBox* commitAfterSaveCheckBox;
    wxTextCtrl* remoteUrlInput;
    wxTextCtrl* logCtrl;
    FileWatcher* watcher;
    std::unique_ptr<GitOps> gitOps;
    bool isTracking;
    std::string repoPath;
    std::string remoteUrl;
    std::string accessToken;  // Store the OAuth access token
};

DECLARE_APP(App)

#endif // LAZY_GIT_APP_HPP