#include "mailbox.h"


namespace GRender
{
    static std::string time2String(const TimePoint& t1, const TimePoint& t2)
    {
        using namespace std::chrono;

        auto us = duration_cast<microseconds>(t2 - t1);

        auto ms = duration_cast<milliseconds>(us);
        us -= duration_cast<microseconds>(ms);
        auto secs = duration_cast<seconds>(ms);
        ms -= duration_cast<milliseconds>(secs);
        auto mins = duration_cast<minutes>(secs);
        secs -= duration_cast<seconds>(mins);
        auto hour = duration_cast<hours>(mins);
        mins -= duration_cast<minutes>(hour);

        std::string txt;

        if (mins.count() > 0)
        {
            if (hour.count() > 0)
                txt += std::to_string(hour.count()) + "h ";

            txt += std::to_string(mins.count()) + "min ";
        }

        float fSecs = float(secs.count()) + 1e-3f * float(ms.count());

        char buf[16];
        sprintf(buf, "%.3f secs ", fSecs);
        txt += buf;

        return txt;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    void Info::show(void)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, { 0.180f, 0.800f, 0.443f, 1.0f });
        ImGui::Text("INFO:");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(content.c_str());
        ImGui::Spacing();
        is_read = true;
    }

    void Warn::show(void)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, { 0.957f, 0.816f, 0.247f, 1.0f });
        ImGui::Text("WARNING:");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(content.c_str());
        ImGui::Spacing();
        is_read = true;
    }

    void Error::show(void)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.0f, 0.0f, 1.0f });
        ImGui::Text("ERROR:");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(content.c_str());
        ImGui::Spacing();
        is_read = true;
    }

    void Progress::show(void)
    {
        if (!is_read)
            current = Clock::now();

        ImGui::Text("%s :: Time: %s", content.c_str(), time2String(zero, current).c_str());

        float width = 0.8f * ImGui::GetContentRegionAvail().x;
        ImGui::ProgressBar(progress, { width, 0 }); // 0 goes for automatic height

        if (!is_read)
        {
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                is_read = true;
                function(ptr);
            }
        }

        ImGui::Spacing();

        if (progress >= 1.0f)
            is_read = true;
    }

    void Timer::show(void)
    {
        if (!is_read)
            current = Clock::now();

        ImGui::Text("%s :: Time: %s", content.c_str(), time2String(zero, current).c_str());

        if (!is_read)
        {
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                stop();
                function(ptr);
            }
        }

        ImGui::Spacing();
    }

    void Timer::stop(void)
    {
        current = Clock::now();
        is_read = true;
    }

    ///////////////////////////////////////////////////////////////////////////

    Mailbox::~Mailbox(void)
    {
        for (Message* msg : messages)
            delete msg;

        messages.clear();
    }

    Info* Mailbox::createInfo(const std::string& msg)
    {
        active = true;
        messages.emplace_back(new Info(msg));
        return reinterpret_cast<Info*>(messages.back());
    }

    Warn* Mailbox::createWarn(const std::string& msg)
    {
        active = true;
        messages.emplace_back(new Warn(msg));
        return reinterpret_cast<Warn*>(messages.back());
    }

    Error* Mailbox::createError(const std::string& msg)
    {
        active = true;
        messages.emplace_back(new Error(msg));
        return reinterpret_cast<Error*>(messages.back());
    }

    Progress* Mailbox::createProgress(const std::string& msg, void (*function)(void*), void* ptr)
    {
        active = true;
        messages.emplace_back(new Progress(msg, function, ptr));
        return reinterpret_cast<Progress*>(messages.back());
    }

    Timer* Mailbox::createTimer(const std::string& msg, void(*function)(void*), void* ptr)
    {
        active = true;
        messages.emplace_back(new Timer(msg, function, ptr));
        return reinterpret_cast<Timer*>(messages.back());
    }

    void Mailbox::showMessages(void)
    {
        if (!active)
            return;

        const ImVec2 workpos = ImGui::GetMainViewport()->WorkPos;
        ImGui::SetNextWindowPos({ workpos.x + 50 * DPI_FACTOR, workpos.y + 40 * DPI_FACTOR }, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({700 * DPI_FACTOR, 400 * DPI_FACTOR }, ImGuiCond_FirstUseEver);

        ImGui::Begin("Mailbox", &active);
        ImGui::SetWindowSize({ DPI_FACTOR * 720.0f, DPI_FACTOR * 405.0f });

        ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.02f, 0.02f, 0.02f, 1.0f });

        ImVec2 size = { 0.98f * ImGui::GetWindowWidth(), 0.8f * ImGui::GetWindowHeight() };
        ImGui::BeginChild("mail_child", size, true, ImGuiWindowFlags_HorizontalScrollbar);

        for (Message* msg : messages)
            msg->show();

        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Spacing();
        if (ImGui::Button("Clear"))
        {
            active = false;
            messages.remove_if([](Message* msg) -> bool {
                if (msg->is_read)
                {
                    delete msg;
                    return true;
                }
                else
                    return false;
                });
        }

        ImGui::SameLine();
        if (ImGui::Button("Close"))
            active = false;

        ImGui::End();
    }
}