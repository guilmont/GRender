#include "dialogImpl.h"

#include <algorithm>
#include "events.h"
#include "mailbox.h"

namespace GRender::dialog::internal {

namespace fs = std::filesystem;

static fs::path getHomeDirectory(void) {
#ifdef WIN32
    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}
    
static bool sortingFunction(const  fs::path& p1, const fs::path& p2) {
    // We want to sort paths so that directories are always on top, hence we have 3 options:
    // 1) Both paths are directories -> Sort alphabetically to lowercase
    // 2) Both paths are files -> Sort alphabetically to lowercase
    // 3) One of each -> directory goes first

    bool
        isDir1 = fs::is_directory(p1),
        isDir2 = fs::is_directory(p2);

    // Case 3
    if (isDir1 && !isDir2) { return true; }
    if (!isDir1 && isDir2) { return false; }

    // Case 1 and 2
    // Getting file names and converting to lowercase
    std::string
        str1 = p1.filename().string(),
        str2 = p2.filename().string();

    std::transform(str1.begin(), str1.end(), str1.begin(), [](char c) { return std::tolower(c); });
    std::transform(str2.begin(), str2.end(), str2.begin(), [](char c) { return std::tolower(c); });

    return str1 < str2;
}


static int inputCompletion(ImGuiInputTextCallbackData* data) {
    // In this function, we determine all the available paths that contain inserted partial path
    // and delete otherwise
    DialogImpl* diag = reinterpret_cast<DialogImpl*>(data->UserData);
    fs::path localPath(data->Buf);

    // First we work on auto completion
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion) {
        const std::string partial = localPath.filename().string();

        // With this partial, we should go to parent path and filter for partial
        localPath = localPath.parent_path();
        diag->main_path = localPath;
        diag->updateAvailablePaths();

        // Removing full paths that don't contain out partial
        auto removeFunction = std::remove_if(diag->availablePaths.begin(), diag->availablePaths.end(),
            [diag, &partial](const fs::path& var) -> bool {
                const std::string local = var.filename().string();
                for (size_t i = 0; i < partial.length(); ++i) {
                    if (partial[i] != local[i]) { return true; }
                }
                // If reached here, we might need to filter files with a different extension
                if (fs::is_regular_file(var) && (var.extension().string().compare(diag->mCurrentExt) != 0)) {
                    return true;
                }
                // It passed all tests, so it should not be removed
                return false;
            });

        // Actual removal
        diag->availablePaths.erase(removeFunction, diag->availablePaths.end());

        // If there is only one path left, we set widget string
        if (diag->availablePaths.size() == 1) {
            fs::path var = diag->availablePaths.front();

            if (fs::is_regular_file(var) && var.extension().string().compare(diag->mCurrentExt) == 0) {
                diag->main_path = var;
                diag->filename = var.stem().string();
                std::string loc = var.string().substr(data->BufTextLen);
                data->InsertChars(data->BufTextLen, loc.c_str());
            }
            else if (fs::is_directory(var)) {
                diag->main_path = var;
                diag->updateAvailablePaths();
                std::string loc = var.string().substr(data->BufTextLen) + std::string{ fs::path::preferred_separator };
                data->InsertChars(data->BufTextLen, loc.c_str());
            }
        }
    }

    // Now we handle the edit callbacks. For the moment we care only if we are deleting big chunks of data
    else if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        
        const bool ctrl = GRender::keyboard::IsDown(GRender::Key::RIGHT_CONTROL)
                        || GRender::keyboard::IsDown(GRender::Key::LEFT_CONTROL);

        const bool backspace = GRender::keyboard::IsDown(GRender::Key::BACKSPACE);

        if (ctrl && backspace) {
            std::string strPath = diag->main_path.string();
            if (strPath.back() == fs::path::preferred_separator) {
                strPath.resize(strPath.size() - 1);
            }
            size_t pos = strPath.find_last_of(fs::path::preferred_separator);
            strPath.resize(pos + 1);
            diag->main_path = strPath;
            diag->updateAvailablePaths();

            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, strPath.c_str());
        }
        
    }

    return 0;
}

DialogImpl* DialogImpl::Instance() {
    static DialogImpl dialog;
    return &dialog;
}

DialogImpl::DialogImpl() : main_path(getHomeDirectory()) {}

void DialogImpl::setMainPath(const fs::path& _path) {
    if (!fs::is_directory(_path)) {
        mailbox::CreateError("Directory doesn't exist -> " + _path.string());
        return;
    }

    main_path = _path;
}

void DialogImpl::openDirectory(const std::string& title, const std::function<void(const fs::path&)>& callback) {
    mActive = true;
    mTitle = title;
    mCallback = callback;
    mExtensions = {};
    mCurrentExt = "";
    filename = "";

    internalShow = &DialogImpl::showOpenDirectory;
    updateAvailablePaths();
}

void DialogImpl::openFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const fs::path&)>& callback) {
    mActive = true;
    mTitle = title;
    mCallback = callback;
    mExtensions = extensions;
    mCurrentExt = "." + extensions.front();
    filename = "";

    internalShow = &DialogImpl::showOpenFile;
    updateAvailablePaths();
}

void DialogImpl::saveFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const fs::path&)>& callback) {
    mActive = true;
    mTitle = title;
    mCallback = callback;
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
    ImGui::SetWindowSize({ mSize.x, mSize.y });

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
        mCallback(main_path);
    }
}

void DialogImpl::showOpenFile(void) {
    ImGui::Begin(mTitle.c_str(), &mActive);
    ImGui::SetWindowSize({ mSize.x, mSize.y });

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
        mCallback(main_path);
        mActive = false;
        main_path = main_path.parent_path(); // going back to parent directory
    }
}

void DialogImpl::showSaveFile(void) {
    ImGui::Begin(mTitle.c_str(), &mActive);
    ImGui::SetWindowSize({ mSize.x, mSize.y });

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
    ImVec2 btnSize = { ImGui::GetFontSize() * 6.0f, 0.0f };
    if ((ImGui::Button("Save", btnSize) || check) && filename.size() > 0) {
        main_path /= (filename + mCurrentExt);
        status = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close"))
        mActive = false;

    ImGui::End();

    // Let's check if file already exists
    if (status) {
        mActive = false;
        mExistsPopup = fs::is_regular_file(main_path);

        if (!mExistsPopup) {
            mCallback(main_path);
            main_path = main_path.parent_path();
        }
    }
}

void DialogImpl::fileExistsPopup(void) {
    bool status = false;
    const std::string& name = main_path.filename().string();

    ImGui::Begin("File exists");
    ImGui::SetWindowSize({ 0.0f, 100.0f });

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
        mCallback(main_path);
    }

    if (!mExistsPopup) {
        main_path = main_path.parent_path();
    }
}

void DialogImpl::updateAvailablePaths(void) {
    availablePaths.clear();

    if (!fs::exists(main_path))
        return;

    for (auto entry : fs::directory_iterator(main_path, fs::directory_options::skip_permission_denied)) {
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

bool DialogImpl::systemDisplay(void) {
    // As this function runs to all dialogs, we can provide a shortcut to close dialogs on escape event
    if (keyboard::IsPressed(Key::ESCAPE)) {
        mActive = false;
    }

    ImGui::Text("Input path:");
    ImGui::SameLine();

    if (ImGui::Button("Home")) {
        main_path = getHomeDirectory();
        updateAvailablePaths();
    }
    ImGui::SameLine();
    if (ImGui::Button("Back") && fs::is_directory(main_path.parent_path())) {
        main_path = main_path.parent_path();
        updateAvailablePaths();
    }

    bool status = false;
    char loc[512] = { 0 };
    const std::string& locMain = main_path.string();
    std::copy(locMain.begin(), locMain.end(), loc);
    ImGui::PushItemWidth(0.97f * ImGui::GetWindowWidth());
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue
                                    | ImGuiInputTextFlags_CallbackCompletion 
                                    | ImGuiInputTextFlags_CallbackEdit;

    if (ImGui::InputText("##MainAdress", loc, 512, flags, inputCompletion, this)) {
        fs::path var(loc);
        if (fs::is_regular_file(var)) {
            if (var.extension().string().compare(mCurrentExt) == 0) {
                main_path = var;
                filename = var.stem().string();
                status = true;
            }
            else {
                main_path = var.parent_path();
                filename = var.stem().string();
                updateAvailablePaths();
            }
        }
        else if (fs::is_directory(var)) {
            main_path = std::move(var);
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
                main_path = apath / "";
                updateAvailablePaths();
                break; // availablePaths will be different, so we stop the loop here
            }
            ImGui::Spacing();
        }
        else if (apath.extension().string().compare(mCurrentExt) == 0) {
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.2f, 0.7f, 0.2f, 1.0f });

            if (ImGui::Selectable(name.c_str(), false)) {
                main_path = apath;
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