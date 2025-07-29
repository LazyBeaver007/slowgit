#include "App.h" 
#include "utils.h"
#include <wx/txtstrm.h>
#include <wx/stdpaths.h>
#include <wx/url.h>
#include <wx/uri.h>
#include <wx/sstream.h>     // For wxStringOutputStream
#include <wx/snglinst.h>    // For wxSingleInstanceChecker
#include <wx/socket.h>      // For wxSocketBase, wxSocketServer, wxIPV4address
#include <wx/protocol/http.h> // For wxHTTP

#include <wx/frame.h>       // For wxFrame
#include <wx/panel.h>       // For wxPanel
#include <wx/sizer.h>       // For wxBoxSizer
#include <wx/dirctrl.h>     // For wxDirPickerCtrl
#include <wx/checkbox.h>    // For wxCheckBox
#include <wx/textctrl.h>    // For wxTextCtrl
#include <wx/button.h>      // For wxButton
#include <wx/stattext.h>    // For wxStaticText
#include <wx/log.h>         // For wxLogError, wxLogMessage
#include <wx/fileconf.h>    // For wxFileConfig (to store access token)

#include <ctime>            // For time_t, time (still needed for other potential uses)
#include <string>           // For std::string, strcspn, std::to_string
#include <sstream>          // For std::stringstream 
#include <memory>           // For std::unique_ptr
#include <algorithm>        // For std::min
#define CURL_STATICLIB 
#include <curl.h>


const wxString GITHUB_CLIENT_ID = "Ov23li8nV5wpQPlISDGp";
const wxString GITHUB_CLIENT_SECRET = "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";
const wxString GITHUB_REDIRECT_URI = "http://localhost:8080/callback";
const wxString GITHUB_AUTH_URL = "https://github.com/login/oauth/authorize";
const wxString GITHUB_TOKEN_URL = "https://github.com/login/oauth/access_token";


bool App::OnInit() {
    if (!wxSocketBase::Initialize()) {
        wxLogError("Failed to initialize socket library.");
        return false;
    }

    
    wxFileConfig config("lazy-git", wxEmptyString, "lazy-git.ini");
    accessToken = config.Read("GitHub/AccessToken", "").ToStdString();
    if (!accessToken.empty()) {
        logCtrl = new wxTextCtrl(nullptr, wxID_ANY, wxEmptyString); // Temporary for logging
        logCtrl->AppendText("[" + get_timestamp() + "] Loaded GitHub access token from config.\n");
        delete logCtrl;
        logCtrl = nullptr;
    }

    wxSingleInstanceChecker* checker = new wxSingleInstanceChecker;
    if (checker->IsAnotherRunning()) {
        wxLogError("Another instance of the application is already running.");
        delete checker;
        return false;
    }

    frame = new wxFrame(nullptr, wxID_ANY, "lazy-git", wxDefaultPosition, wxSize(800, 600));
    wxPanel* panel = new wxPanel(frame, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    dirPicker = new wxDirPickerCtrl(panel, wxID_ANY, wxEmptyString, "Select a folder", wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_DIR_MUST_EXIST);
    sizer->Add(dirPicker, 0, wxEXPAND | wxALL, 10);

    commitAfterSaveCheckBox = new wxCheckBox(panel, wxID_ANY, "Commit after save");
    commitAfterSaveCheckBox->SetValue(true);
    sizer->Add(commitAfterSaveCheckBox, 0, wxALL, 10);

    wxBoxSizer* remoteUrlSizer = new wxBoxSizer(wxHORIZONTAL);
    remoteUrlSizer->Add(new wxStaticText(panel, wxID_ANY, "Remote URL: "), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    remoteUrlInput = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1));
    remoteUrlSizer->Add(remoteUrlInput, 1, wxEXPAND | wxALL, 5);
    sizer->Add(remoteUrlSizer, 0, wxEXPAND | wxALL, 10);

    startStopButton = new wxButton(panel, wxID_ANY, "Start Tracking");
    sizer->Add(startStopButton, 0, wxALIGN_CENTER | wxALL, 10);

    gitHubLoginButton = new wxButton(panel, wxID_ANY, "Login with GitHub");
    sizer->Add(gitHubLoginButton, 0, wxALIGN_CENTER | wxALL, 10);

    logCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    sizer->Add(logCtrl, 1, wxEXPAND | wxALL, 10);

    panel->SetSizer(sizer);
    frame->Centre();
    frame->Show(true);

    startStopButton->Bind(wxEVT_BUTTON, &App::OnToggleTracking, this);
    gitHubLoginButton->Bind(wxEVT_BUTTON, &App::OnGitHubLogin, this);

    isTracking = false;
    watcher = nullptr;
    return true;
}

void App::OnToggleTracking(wxCommandEvent& event) {
    if (!isTracking) {
        wxString path = dirPicker->GetPath();
        if (path.empty()) {
            logCtrl->AppendText("[" + get_timestamp() + "] Please select a folder\n");
            return;
        }
        repoPath = path.ToStdString();

        remoteUrl = remoteUrlInput->GetValue().ToStdString();
        if (remoteUrl.empty()) {
            logCtrl->AppendText("[" + get_timestamp() + "] Please enter a remote URL\n");
            return;
        }

        if (accessToken.empty()) {
            logCtrl->AppendText("[" + get_timestamp() + "] INFO: GitHub access token is not set. Operations requiring authentication may fail.\n");
        }

        try {
            gitOps = std::make_unique<GitOps>(repoPath, remoteUrl, logCtrl, accessToken);
            logCtrl->AppendText("[" + get_timestamp() + "] Initial Git Logs:\n" + gitOps->get_git_logs() + "\n");
        }
        catch (const std::exception& e) {
            logCtrl->AppendText("[" + get_timestamp() + "] Error initializing Git repository: " + std::string(e.what()) + "\n");
            return;
        }

        bool commitAfterSave = commitAfterSaveCheckBox->GetValue();
        watcher = new FileWatcher(repoPath, commitAfterSave, *gitOps, logCtrl);
        watcher->startWatching();
        startStopButton->SetLabel("Stop Tracking");
        logCtrl->AppendText("[" + get_timestamp() + "] Started tracking folder: " + repoPath + "\n");
    }
    else {
        if (watcher) {
            watcher->stopWatching();
            delete watcher;
            watcher = nullptr;
        }
        gitOps.reset();
        startStopButton->SetLabel("Start Tracking");
        logCtrl->AppendText("[" + get_timestamp() + "] Stopped tracking folder\n");
    }
    isTracking = !isTracking;
}

void App::OnGitHubLogin(wxCommandEvent& event) {
    wxString authUrl = GITHUB_AUTH_URL + "?client_id=" + GITHUB_CLIENT_ID +
        "&redirect_uri=" + GITHUB_REDIRECT_URI +
        "&scope=repo user";

    if (!wxLaunchDefaultBrowser(authUrl)) {
        logCtrl->AppendText("[" + get_timestamp() + "] Failed to open browser for GitHub login.\n");
        return;
    }
    logCtrl->AppendText("[" + get_timestamp() + "] Opened browser for GitHub login. Waiting for callback...\n");

    wxIPV4address addr;
    addr.Hostname("localhost");
    addr.Service(8080);

    wxSocketServer* server = new wxSocketServer(addr, wxSOCKET_NOWAIT);
    if (!server->IsOk()) {
        logCtrl->AppendText("[" + get_timestamp() + "] Failed to start callback server.\n");
        delete server;
        return;
    }

    wxSocketBase* client = server->Accept(true);
    if (client && client->IsConnected()) {
        char buffer[4096];
        client->Read(buffer, sizeof(buffer) - 1);
        buffer[client->LastCount()] = '\0';
        std::string request(buffer);

        size_t codePos = request.find("code=");
        if (codePos != std::string::npos) {
            size_t ampPos = request.find('&', codePos);
            size_t spacePos = request.find(' ', codePos);
            size_t codeEnd = request.length();
            if (ampPos != std::string::npos) codeEnd = ampPos;
            if (spacePos != std::string::npos && spacePos < codeEnd) codeEnd = spacePos;

            std::string code = request.substr(codePos + 5, codeEnd - (codePos + 5));
            logCtrl->AppendText("[" + get_timestamp() + "] Received authorization code.\n");

            // Use libcurl to exchange code for token
            CURL* curl = curl_easy_init();
            if (curl) {
                std::string postData = "client_id=" + GITHUB_CLIENT_ID.ToStdString() +
                    "&client_secret=" + GITHUB_CLIENT_SECRET.ToStdString() +
                    "&code=" + code +
                    "&redirect_uri=" + GITHUB_REDIRECT_URI.ToStdString();

                std::string response;
                curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/login/oauth/access_token");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, "Accept: application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
                    ((std::string*)userp)->append((char*)contents, size * nmemb);
                    return size * nmemb;
                    });
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                CURLcode res = curl_easy_perform(curl);
                if (res == CURLE_OK) {
                    logCtrl->AppendText("[" + get_timestamp() + "] GitHub token response: " + wxString(response) + "\n");

                    size_t tokenStart = response.find("\"access_token\":\"");
                    if (tokenStart != std::string::npos) {
                        tokenStart += strlen("\"access_token\":\"");
                        size_t tokenEnd = response.find("\"", tokenStart);
                        if (tokenEnd != std::string::npos) {
                            accessToken = response.substr(tokenStart, tokenEnd - tokenStart);
                            logCtrl->AppendText("[" + get_timestamp() + "] Successfully authenticated. Token stored.\n");

                            wxFileConfig config("lazy-git", wxEmptyString, "lazy-git.ini");
                            config.Write("GitHub/AccessToken", wxString(accessToken));
                            config.Flush();
                        }
                    }
                    else {
                        logCtrl->AppendText("[" + get_timestamp() + "] Failed to parse access token from response.\n");
                    }
                }
                else {
                    logCtrl->AppendText("[" + get_timestamp() + "] curl error: " + wxString(curl_easy_strerror(res)) + "\n");
                }

                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);
            }
        }
        else {
            logCtrl->AppendText("[" + get_timestamp() + "] No code received in callback.\n");
        }

        std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
            "<html><head><title>Authentication Success</title></head><body><h1>Authentication Successful!</h1>"
            "<p>You may now close this window and return to the app.</p></body></html>";
        client->Write(httpResponse.c_str(), httpResponse.size());
        client->Destroy();
    }
    else {
        if (client) client->Destroy();
        logCtrl->AppendText("[" + get_timestamp() + "] Failed to accept GitHub callback connection.\n");
    }

    server->Close();
    delete server;
}


wxIMPLEMENT_APP(App);