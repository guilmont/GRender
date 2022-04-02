#include "dialog.h"

namespace GRender
{

    Dialog::Dialog(void)
    {
#ifdef WIN32
        mainPath = std::getenv("USERPROFILE");
#else
        mainPath = std::getenv("HOME");
#endif
    }

    Dialog::~Dialog(void) {}

    void Dialog::createDialog(GDialog type, const std::string& title, const std::list<std::string>& ext, void* data, void (*callback)(const fs::path&, void*))
    {
        active = true;
        this->title = title;
        this->lExtension = ext;
        this->currentExt = "." + ext.front();
        this->filename = "";

        switch (type)
        {
        case GDialog::OPEN:
            dialog_function = &Dialog::openDialog;
            break;
        case GDialog::SAVE:
            dialog_function = &Dialog::saveDialog;
            break;
        default:
            pout("ERROR: Unknown dialog type");
            exit(-1);
            break;
        }

        this->callback = callback;
        this->callback_data = data;

    } // createDialog

    void Dialog::showDialog(void)
    {
        if (!active)
            return;


        ImGui::Begin(title.c_str(), &active);
        ImGui::SetWindowSize({ DPI_FACTOR * size.x, DPI_FACTOR * size.y });

        bool status = (this->*dialog_function)();
        ImGui::End();

        if (existPopup)
            status = fileExistsPopup();

        if (status)
        {
            active = false;
            callback(filePath.string(), callback_data);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // PRIVATE FUNCTIONS

    bool Dialog::openDialog(void)
    {
        ImGui::Text("Input path to file:");

        ImGui::SameLine();
        if (ImGui::Button("Back"))
            mainPath = mainPath.parent_path();

        static char loc[512] = { 0 };
        sprintf(loc, "%s", mainPath.string().c_str());

        float width = ImGui::GetContentRegionAvail().x;

        ImGui::PushItemWidth(width);
        if (ImGui::InputText("##MainAdress", loc, 512, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            fs::path var(loc);
            if (fs::is_directory(var))
                mainPath = std::move(var);

            else if (fs::is_regular_file(var))
                mainPath = var.parent_path();
        }
        ImGui::PopItemWidth();

        bool status = systemDisplay();


        ImGui::Text("Extensions:");
        ImGui::SameLine();

        ImGui::PushItemWidth(100);
        if (ImGui::BeginCombo("##combo", currentExt.c_str() + 1))
        {
            for (const std::string& ext : lExtension)
            {
                bool check = currentExt.compare("." + ext) == 0;
                if (ImGui::Selectable(ext.c_str(), &check))
                {
                    currentExt = "." + ext;
                    ImGui::SetItemDefaultFocus();
                } // if-selected
            }

            ImGui::EndCombo();
        }

        ImGui::PopItemWidth();

        ImGui::SameLine();

        if (ImGui::Button("Close"))
            active = false;

        return status;

    } // openDialog

    bool Dialog::saveDialog(void)
    {
        bool status = false;

        ImGui::Text("Output path:");

        ImGui::SameLine();
        if (ImGui::Button("Back"))
            mainPath = mainPath.parent_path();

        char buf[512] = { 0 };
        sprintf(buf, "%s", mainPath.string().c_str());

        float width = ImGui::GetContentRegionAvail().x;

        ImGui::PushItemWidth(width);
        if (ImGui::InputText("##MainAdress", buf, 512, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            fs::path var(buf);
            if (fs::is_directory(var))
                mainPath = std::move(var);

            else if (fs::is_regular_file(var))
            {
                mainPath = var.parent_path();
                filename = var.stem();
            }
        }
        ImGui::PopItemWidth();


        if (systemDisplay())
            filename = filePath.stem();

        ImGui::Text("Filename");
        ImGui::SameLine();

        memset(buf, 0, 512);
        sprintf(buf, "%s", filename.string().c_str());

        ImGui::PushItemWidth(0.333f * width);
        ImGui::InputText("##inp", buf, 512, ImGuiInputTextFlags_EnterReturnsTrue);
        filename = fs::path(buf).stem();

        ImGui::SameLine();

        ImGui::PushItemWidth(100);
        if (ImGui::BeginCombo("##combo", currentExt.c_str() + 1))
        {
            for (const std::string& ext : lExtension)
            {
                bool check = currentExt.compare("." + ext) == 0;
                if (ImGui::Selectable(ext.c_str(), &check))
                {
                    currentExt = "." + ext;
                    ImGui::SetItemDefaultFocus();
                } // if-selected
            }

            ImGui::EndCombo();
        }

        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        bool check = ImGui::GetIO().KeysDown[GLFW_KEY_ENTER] && filename.string().size() > 0;
        if (ImGui::Button("Save") || check)
        {
            filePath = mainPath / (filename.string() + currentExt);
            status = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Close"))
            active = false;


        // Let's check if file already exists
        if (status)
            existPopup = fs::is_regular_file(filePath);


        return status;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool Dialog::systemDisplay(void)
    {
        bool status = false;

        float width = ImGui::GetContentRegionAvail().x * DPI_FACTOR;

        ImGui::BeginChild("child_2", { width, 0.44f * width }, true);

        for (auto entry : fs::directory_iterator(mainPath, fs::directory_options::skip_permission_denied))
        {
            const fs::path& path = entry.path();
            const std::string& filename = path.filename().string();

            // hidden files
            if (filename[0] == '.')
                continue;

            bool isDirectory = false, isFile = false;
            try
            {
                isDirectory = fs::is_directory(path);
                isFile = fs::is_regular_file(path);
            }
            catch (...)
            {
            }

            if (isDirectory)
            {
                if (ImGui::Selectable(filename.c_str(), true))
                    mainPath /= filename;

                ImGui::Spacing();

            }

            if (isFile && path.extension().compare(currentExt) == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.2f, 0.7f, 0.2f, 1.0f });
                if (ImGui::Selectable(filename.c_str(), false))
                {
                    filePath = mainPath / filename;
                    status = true;
                }

                ImGui::PopStyleColor();

                ImGui::Spacing();
            }

        }


        ImGui::EndChild();

        ImGui::Spacing();
        ImGui::Spacing();

        if (drop.handle)
        {
            if (fs::is_directory(drop.path))
                mainPath = drop.path;

            else if (fs::is_regular_file(drop.path))
                mainPath = drop.path.parent_path();

            drop.handle = false;
        }

        return status;
    } // systemDisplay

    bool Dialog::fileExistsPopup(void)
    {
        bool status = false;
        ImGui::OpenPopup("File exists");

        if (ImGui::BeginPopupModal("File exists"))
        {
            const std::string& name = mainPath.filename().string();
            ImGui::Text("'%s' already exists. Replace?", name.c_str());

            if (ImGui::Button("Yes"))
            {
                existPopup = active = false;
                status = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("No"))
            {
                existPopup = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        } // popup-file-exists

        return status;
    }

}