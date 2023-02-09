#include "core.h"

namespace GRender::dialog::internal {
    namespace fs = std::filesystem;
    // Used
    int inputCompletion(ImGuiInputTextCallbackData* data);

    class DialogImpl {
    public:
        DialogImpl(void);
        ~DialogImpl(void) = default;

        void setMainPath(const fs::path& mainPath);

        void openDirectory(const std::string& title, void (*callback)(const fs::path&, void*), void* data);
        void openFile(const std::string& title, const std::vector<std::string>& extensions, void (*callback)(const fs::path&, void*), void* data);
        void saveFile(const std::string& title, const std::vector<std::string>& extensions, void (*callback)(const fs::path&, void*), void* data);

        void showDialog();

    private:
        // Gets all directories and files available at current path.
        void updateAvailablePaths(void);

        // Specialied windows
        void showOpenDirectory(void);
        void showOpenFile(void);
        void showSaveFile(void);
        void fileExistsPopup(void);

        // Child widget with folders and files
        bool systemDisplay(void);

    private:
        // window size
        const glm::vec2 mSize = { 720.0f, 480.0f };

        // internal control
        bool mActive = false;
        bool mExistsPopup = false;
        void (DialogImpl::* internalShow)(void) = nullptr;

        // titles and extensions
        std::string mTitle;
        std::vector<std::string> mExtensions;
        std::string mCurrentExt = "";

        // callback info
        void* mCallbackData = nullptr;
        void (*mCallback)(const std::filesystem::path&, void*) = nullptr;

        // filepaths variables
        std::string filename = "";
        std::filesystem::path mainpath;
        std::vector<std::filesystem::path> availablePaths;
        friend int inputCompletion(ImGuiInputTextCallbackData* data);
    };
} // namespace GRender