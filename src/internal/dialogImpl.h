#include "core.h"

namespace GRender::dialog::internal {
    extern int inputCompletion(ImGuiInputTextCallbackData* data);

    class DialogImpl {
        friend int inputCompletion(ImGuiInputTextCallbackData* data);

    public:
        static DialogImpl* Instance();

        DialogImpl(const DialogImpl&) = delete;
        DialogImpl& operator=(const DialogImpl&) = delete;

        void setMainPath(const fs::path& mainPath);

        void openDirectory(const std::string& title, const std::function<void(const fs::path&)>& callback);
        void openFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const fs::path&)>& callback);
        void saveFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const fs::path&)>& callback);

        void showDialog();

    private:
        DialogImpl();
        ~DialogImpl(void) = default;

        // Gets all directories and files available at current path.
        void updateAvailablePaths(void);

        // Specialized windows
        void showOpenDirectory(void);
        void showOpenFile(void);
        void showSaveFile(void);
        void fileExistsPopup(void);

        // Child widget with folders and files
        bool systemDisplay(void);

    private:
        // window size
        const glm::vec2 m_Size = { 720.0f, 480.0f };

        // internal control
        bool m_Active = false;
        bool m_ExistsPopup = false;
        void (DialogImpl::* internalShow)(void) = nullptr;

        // titles and extensions
        std::string m_Title;
        std::vector<std::string> m_Extensions;
        std::string m_CurrentExt = "";

        // callback info
        std::function<void(const std::filesystem::path&)> m_Callback = nullptr;

        // File path variables
        std::string m_Filename = "";
        std::filesystem::path m_MainPath;
        std::vector<std::filesystem::path> m_AvailablePaths;
    };
} // namespace GRender