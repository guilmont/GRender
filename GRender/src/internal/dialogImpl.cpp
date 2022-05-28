#include "dialogImpl.h"

#include <algorithm>
#include "events.h"

namespace fs = std::filesystem;

static fs::path getHomeDirectory(void) {
#ifdef WIN32
    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}

static bool sortingFunction(const  fs::path &p1, const fs::path& p2) {
    // We want to sort paths so that directories are always on top, hence we have 3 options:
    // 1) Both paths are directories -> Sort alphabetically to lowercase
    // 2) Both paths are files -> Sort alphabetically to lowercase
    // 3) One of each -> directory goes first

    bool
        isDir1 = fs::is_directory(p1),
        isDir2 = fs::is_directory(p2);

    // Case 3
    if (isDir1 && !isDir2) { return true; }
    if (!isDir1 && isDir2) {return false; }

    // Case 1 and 2
    // Getting file names and converting to lowercase
    std::string 
        str1 = p1.filename().string(),
        str2 = p2.filename().string();

    std::transform(str1.begin(), str1.end(), str1.begin(), [](char c) { return std::tolower(c); });
    std::transform(str2.begin(), str2.end(), str2.begin(), [](char c) { return std::tolower(c); });

    return str1 < str2;
}

namespace GRender::dialog::internal {

DialogImpl::DialogImpl() : mainpath(getHomeDirectory()) {}

void DialogImpl::openDirectory(const std::string& title,
                               void (*callback)(const fs::path&, void*), void* data) {
    mActive = true;
    mTitle = title;
    mCallback = callback;
    mCallbackData = data;
    mExtensions = {};
    mCurrentExt = "";
    filename = "";

    internalShow = &DialogImpl::showOpenDirectory;
    updateAvailablePaths();
}

void DialogImpl::openFile(const std::string& title, const std::vector<std::string>& extensions,
                          void (*callback)(const fs::path&, void*), void* data) {
    mActive = true;
    mTitle = title;
    mCallback = callback;
    mCallbackData = data;
    mExtensions = extensions;
    mCurrentExt = "." + extensions.front();
    filename = "";

    internalShow = &DialogImpl::showOpenFile;
    updateAvailablePaths();
}

void DialogImpl::saveFile(const std::string& title, const std::vector<std::string>& extensions,
                          void (*callback)(const fs::path&, void*), void* data) {
    mActive = true;
    mTitle = title;
    mCallback = callback;
    mCallbackData = data;
    mExtensions = extensions;
    mCurrentExt = "." + extensions.front();
    filename = "";

    internalShow = &DialogImpl::showSaveFile;
    updateAvailablePaths();
}

void DialogImpl::showDialog() {
    if (mExistsPopup)
        fileExistsPopup();

    if (mActive)
        (this->*internalShow)();
}

void DialogImpl::showOpenDirectory(void) {
    ImGui::Begin(mTitle.c_str(), &mActive);
    ImGui::SetWindowSize({ DPI_FACTOR * mSize.x, DPI_FACTOR * mSize.y });

    bool status = systemDisplay();

    if (ImGui::Button("Open")) {
        mActive = false;
        status = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close"))
        mActive = false;

    ImGui::End();

    if (status) {
        mActive = false;
        mCallback(mainpath, mCallbackData);
    }
}

void DialogImpl::showOpenFile(void) {
    ImGui::Begin(mTitle.c_str(), &mActive);
    ImGui::SetWindowSize({ DPI_FACTOR * mSize.x, DPI_FACTOR * mSize.y });

    bool status = systemDisplay();

    ImGui::Text("Extensions:");
    ImGui::SameLine();

    ImGui::PushItemWidth(100);
    if (ImGui::BeginCombo("##combo", mCurrentExt.c_str() + 1)) { // skipping the point
        for (const std::string& ext : mExtensions) {
            bool check = mCurrentExt.compare(ext) == 0;

            if (ImGui::Selectable(ext.c_str(), &check)) {
                mCurrentExt = "." + ext;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Close"))
        mActive = false;

    ImGui::End();

    if (status) {
        mCallback(mainpath, mCallbackData);
        mActive = false;
        mainpath = mainpath.parent_path(); // going back to parent directory
    }
}

void DialogImpl::showSaveFile(void) {
    ImGui::Begin(mTitle.c_str(), &mActive);
    ImGui::SetWindowSize({ DPI_FACTOR * mSize.x, DPI_FACTOR * mSize.y });

    bool status = systemDisplay();

    ImGui::Text("Filename");
    ImGui::SameLine();

    char buf[512] = { 0 };
    std::copy(filename.begin(), filename.end(), buf);

    ImGui::PushItemWidth(0.333f * ImGui::GetWindowWidth());
    if (ImGui::InputText("##inp", buf, 512)) {
        filename = fs::path(buf).stem().string();
    }
    ImGui::SameLine();

    ImGui::PushItemWidth(100);
    if (ImGui::BeginCombo("##combo", mCurrentExt.c_str() + 1)) {
        for (const std::string& ext : mExtensions) {
            bool check = mCurrentExt.compare("." + ext) == 0;
            if (ImGui::Selectable(ext.c_str(), &check)) {
                mCurrentExt = "." + ext;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    bool check = ImGui::IsKeyDown(ImGuiKey_Enter);
    ImVec2 btnSize = {ImGui::GetFontSize() * 6.0f, 0.0f };
    if ((ImGui::Button("Save", btnSize) || check) && filename.size() > 0) {
        mainpath /= (filename + mCurrentExt);
        status = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close"))
        mActive = false;

    ImGui::End();

    // Let's check if file already exists
    if (status) {
        mActive = false;
        mExistsPopup = fs::is_regular_file(mainpath);

        if (!mExistsPopup) {
            mCallback(mainpath, mCallbackData);
            mainpath = mainpath.parent_path();
        }
    }
}

void DialogImpl::fileExistsPopup(void) {
    bool status = false;
    const std::string& name = mainpath.filename().string();

    ImGui::Begin("File exists");
    ImGui::SetWindowSize({ 0.0f, DPI_FACTOR * 100.0f });

    ImGui::Text("'%s' already exists. Replace?", name.c_str());

    if (ImGui::Button("Yes")) {
        status = true;
        mExistsPopup = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("No")) {
        mExistsPopup = status = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::End();

    if (status) {
        mCallback(mainpath, mCallbackData);
    }

    if (!mExistsPopup) {
        mainpath = mainpath.parent_path();
    }
}

void DialogImpl::updateAvailablePaths(void) {
    availablePaths.clear();

    for (auto entry : fs::directory_iterator(mainpath, fs::directory_options::skip_permission_denied)) {
        const fs::path& path = entry.path();

        try {
            const std::string& filename = path.filename().string();
            bool check = fs::is_directory(path);
            check = check || (mExtensions.size() > 0 && fs::is_regular_file(path));  // only interesting if we have extensions
            check = check && (filename[0] != '.');  // not interested in hidden files for now

            if (check) {
                availablePaths.emplace_back(path);
            }
        }
        catch (...) {
            // Something weird happened and we are probably not interest in that anyway
        }
    }

    // Sorting paths alphabetically
    std::sort(availablePaths.begin(), availablePaths.end(), sortingFunction);
}

int inputCompletion(ImGuiInputTextCallbackData* data) {
    // In this function, we determine all the available paths that contain inserted partial path
    // and delete otherwise
    DialogImpl* diag = reinterpret_cast<DialogImpl*>(data->UserData);
    fs::path localPath(data->Buf);
    std::string partial = localPath.filename().string();

    // With this partial, we should go to parent path and filter for partial
    localPath = localPath.parent_path();
    diag->mainpath = localPath;
    diag->updateAvailablePaths();
    
    ///////////////////////////////////////////////////////
    // Removing different paths
    //
    // Setup a lambda function which determines simular partial paths
    auto removeFunction = std::remove_if(diag->availablePaths.begin(), diag->availablePaths.end(),
        [&partial](const fs::path& var) -> bool {
            std::string loc = var.filename().string();

            for (size_t k = 0; k < partial.size(); k++) {
                if (loc[k] != partial[k]) { return true; }
            }
            return false;
        });

    // Actual removal
    diag->availablePaths.erase(removeFunction, diag->availablePaths.end());

    ///////////////////////////////////////////////////////

    // If there is only one path left, we set widget string
    if (diag->availablePaths.size() == 1) {
        fs::path var = diag->availablePaths.front();

        if (fs::is_regular_file(var) && var.extension().string().compare(diag->mCurrentExt) == 0) {
            diag->mainpath = var;
            diag->filename = var.stem().string();
            std::string loc = var.string().substr(data->BufTextLen);
            data->InsertChars(data->BufTextLen, loc.c_str());
        }
        else if (fs::is_directory(var)) {
            diag->mainpath = var;
            diag->updateAvailablePaths();
            std::string loc = var.string().substr(data->BufTextLen);
            data->InsertChars(data->BufTextLen, loc.c_str());
        }
    }

    return 0;
}

bool DialogImpl::systemDisplay(void) {

    ImGui::Text("Input path:");
    ImGui::SameLine();

    if (ImGui::Button("Home")) {
        mainpath = getHomeDirectory();
        updateAvailablePaths();
    }
    ImGui::SameLine();
    if (ImGui::Button("Back") && fs::is_directory(mainpath.parent_path())) {
        mainpath = mainpath.parent_path();
        updateAvailablePaths();
    }

    bool status = false;
    char loc[512] = { 0 };
    const std::string& locMain = mainpath.string();
    std::copy(locMain.begin(), locMain.end(), loc);
    ImGui::PushItemWidth(0.97f * ImGui::GetWindowWidth());
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue
                              | ImGuiInputTextFlags_CallbackCompletion;
    
    if (ImGui::InputText("##MainAdress", loc, 512, flags, inputCompletion, this)) {
        fs::path var(loc);
        if (fs::is_regular_file(var)) {
            if (var.extension().string().compare(mCurrentExt) == 0) {
                mainpath = var;
                filename = var.stem().string();
                status = true;
            }
            else {
                mainpath = var.parent_path();
                filename = var.stem().string();
                updateAvailablePaths();
            }
        }
        else if (fs::is_directory(var)) {
            mainpath = std::move(var);
            updateAvailablePaths();
        }
    }
    ImGui::PopItemWidth();

    ImVec2 size = { 0.97f * ImGui::GetWindowWidth(), 0.70f * ImGui::GetWindowHeight() };
    ImGui::BeginChild("child_2", size, true, ImGuiWindowFlags_HorizontalScrollbar);

    for (const fs::path& apath : availablePaths) {
        const std::string& name = apath.filename().string();

        if (fs::is_directory(apath)) {
            if (ImGui::Selectable(name.c_str(), true)) {
                mainpath = apath;
                updateAvailablePaths();
                break; // availablePaths will be different, so we stop the loop here
            }
            ImGui::Spacing();
        }
        else if (apath.extension().string().compare(mCurrentExt) == 0) {
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.2f, 0.7f, 0.2f, 1.0f });

            if (ImGui::Selectable(name.c_str(), false)) {
                mainpath = apath;
                filename = name;
                status = true;
            }

            ImGui::PopStyleColor();
            ImGui::Spacing();
        }
    }
    ImGui::EndChild();
    ImGui::Spacing();
    ImGui::Spacing();

    return status;
}

} // namespace GRender