#include "dialog.h"

namespace fs = std::filesystem;

static fs::path getHomeDirectory(void) {
#ifdef WIN32
    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}

namespace GRender::dialog {

// window size
constexpr glm::vec2 mSize = { 720.0f, 480.0f };

// internal control
static bool mActive = false;
static bool mExistsPopup = false;
static void (*internalShow)(void) = nullptr;

// titles and extensions
static std::string mTitle;
static std::vector<std::string> mExtensions;
static std::string mCurrentExt = "";

// callback info
static void* mCallbackData = nullptr;
static void (*mCallback)(const std::filesystem::path&, void*) = nullptr;

// filepaths variables
static std::string filename = "";
static std::filesystem::path mainpath = getHomeDirectory();
static std::vector<std::filesystem::path> availablePaths;

static void updateAvailablePaths(void) {
    // Gets all directories and files available at current path. 
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
}

static bool systemDisplay(void) {
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

    char loc[512] = { 0 };
    const std::string& locMain = mainpath.string();
    std::copy(locMain.begin(), locMain.end(), loc);
    ImGui::PushItemWidth(0.97f * ImGui::GetWindowWidth());
    if (ImGui::InputText("##MainAdress", loc, 512, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs::path var(loc);
        if (fs::is_regular_file(var)) {
            mainpath = var.parent_path();
            filename = var.stem().string();
            updateAvailablePaths();
        }
        else if (fs::is_directory(var)) {
            mainpath = std::move(var);
            updateAvailablePaths();
        }
    }
    ImGui::PopItemWidth();

    bool status = false;
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

static void showOpenDirectory(void) {
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

static void showOpenFile(void) {
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

static void showSaveFile(void) {
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
    if ((ImGui::Button("Save") || check) && filename.size() > 0) {
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

static void fileExistsPopup(void) {
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

void OpenDirectory(const std::string& title, void (*callback)(const fs::path&, void*), void* data) {

    mActive = true;
    mTitle = title;
    mCallback = callback;
    mCallbackData = data;
    mExtensions = {};
    mCurrentExt = "";
    filename = "";

    internalShow = &showOpenDirectory;
    updateAvailablePaths();
}

void OpenFile(const std::string& title, const std::vector<std::string>& extensions, 
                       void (*callback)(const fs::path&, void*), void* data) {

    mActive = true;
    mTitle = title;
    mCallback = callback;
    mCallbackData = data;
    mExtensions = extensions;
    mCurrentExt = "." + extensions.front();
    filename = "";

    internalShow = &showOpenFile;
    updateAvailablePaths();
}

void SaveFile(const std::string& title, const std::vector<std::string>& extensions,
                      void (*callback)(const fs::path&, void*), void* data) {

    mActive = true;
    mTitle = title;
    mCallback = callback;
    mCallbackData = data;
    mExtensions = extensions;
    mCurrentExt = "." + extensions.front();
    filename = "";
    
    internalShow = &showSaveFile;
    updateAvailablePaths();
}

void ShowDialog() {
    if (mExistsPopup)
        fileExistsPopup();

    if (mActive)
        (*internalShow)();
}


} // namespace GRender