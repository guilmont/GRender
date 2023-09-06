#include "mailbox.h"

namespace GRender {

static std::string time2String(const TimePoint& t1, const TimePoint& t2) {
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
    if (mins.count() > 0) {
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

/////////////////////////////

Info::Info(const std::string& msg) : Message(msg) {}

void Info::show(void) {
    ImGui::PushStyleColor(ImGuiCol_Text, { 0.180f, 0.800f, 0.443f, 1.0f });
    ImGui::Text("INFO:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextUnformatted(content.c_str());
    ImGui::Spacing();
    is_read = true;
}

/////////////////////////////

Warn::Warn(const std::string& msg) : Message(msg) {}

void Warn::show(void) {
    ImGui::PushStyleColor(ImGuiCol_Text, { 0.957f, 0.816f, 0.247f, 1.0f });
    ImGui::Text("WARNING:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextUnformatted(content.c_str());
    ImGui::Spacing();
    is_read = true;
}

/////////////////////////////

Error::Error(const std::string& msg) : Message(msg) {}

void Error::show(void) {
    ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.0f, 0.0f, 1.0f });
    ImGui::Text("ERROR:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextUnformatted(content.c_str());
    ImGui::Spacing();
    is_read = true;
}

/////////////////////////////

Progress::Progress(const std::string& msg, const std::function<void(void)>& cancelFunction)
    : Message(msg), m_CancelFunction(cancelFunction), zero(Clock::now()) {}

void Progress::show(void) {
    if (!is_read)
        current = Clock::now();

    ImGui::Text("%s :: Time: %s", content.c_str(), time2String(zero, current).c_str());

    float width = 0.8f * ImGui::GetContentRegionAvail().x;
    ImGui::ProgressBar(progress, { width, 0 }); // 0 goes for automatic height

    if (!is_read) {
        ImGui::SameLine();
        if (m_CancelFunction && ImGui::Button("Cancel")) {
            is_read = true;
            m_CancelFunction();
        }
    }

    ImGui::Spacing();
    if (progress >= 1.0f) {
        is_read = true;
        progress = 1.0f;
    }
}

/////////////////////////////

Timer::Timer(const std::string& msg, const std::function<void(void)>& cancelFunction)
    : Message(msg), m_CancelFunction(cancelFunction), zero(Clock::now()) {}

void Timer::show(void) {
    if (!is_read)
        current = Clock::now();

    ImGui::Text("%s :: Time: %s", content.c_str(), time2String(zero, current).c_str());

    if (!is_read) {
        ImGui::SameLine();
        if (m_CancelFunction && ImGui::Button("Cancel")) {
            stop();
            m_CancelFunction();
        }
    }

    ImGui::Spacing();
}

void Timer::stop(void) {
    current = Clock::now();
    is_read = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace mailbox::internal {

class MailboxData {
public:
    static MailboxData* Instance() {
        static MailboxData data;
        return &data;
    }

    MailboxData(const MailboxData&) = delete;
    MailboxData& operator=(const MailboxData&) = delete;

    template <typename TP, typename... Args>
    TP* createMessage(Args... args);

    void open(void) { m_Active = true; }
    void close(void) { m_Active = false; }

    void showMessages();
    void clear();

private:
    MailboxData(void) = default;
    ~MailboxData(void);

    bool m_Active = false;
    std::list<Message*> m_Messages;

};

/////////////////////////////

MailboxData::~MailboxData() {
    for (Message* msg : m_Messages) {
        delete msg;
    }
    m_Messages.clear();
}

template <typename TP, typename... Args>
TP* MailboxData::createMessage(Args... args) {
    open();
    m_Messages.emplace_back(new TP(args...));
    return reinterpret_cast<TP*>(m_Messages.back());
}

void MailboxData::showMessages() {
    if (!m_Active)
        return;

    ImGui::Begin("Mailbox", &m_Active);
    ImGui::SetWindowSize({ 720.0f, 405.0f });

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.02f, 0.02f, 0.02f, 1.0f });

    ImVec2 size = { 0.98f * ImGui::GetWindowWidth(), 0.82f * ImGui::GetWindowHeight() };
    ImGui::BeginChild("mail_child", size, true, ImGuiWindowFlags_HorizontalScrollbar);

    for (Message* msg : m_Messages) {
        msg->show();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    if (ImGui::Button("Close"))
        Close();

    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        Clear();

    ImGui::End();
}

void MailboxData::clear() {
    m_Messages.remove_if(
        [](Message* msg) -> bool {
            if (msg->is_read) {
                delete msg;
                return true;
            }
            else {
                return false;
            }
        }
    );
}

} //namespace mailbox::internal

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace mailbox {

void Open() {
    internal::MailboxData::Instance()->open();
}

void Close() {
    internal::MailboxData::Instance()->close();
}

Info* CreateInfo(const std::string& msg) {
    return internal::MailboxData::Instance()->createMessage<Info>(msg);
}

Warn* CreateWarn(const std::string& msg) {
    return internal::MailboxData::Instance()->createMessage<Warn>(msg);
}

Error* CreateError(const std::string& msg) {
    return internal::MailboxData::Instance()->createMessage<Error>(msg);
}

Progress* CreateProgress(const std::string& msg, const std::function<void(void)>& cancelFunction) {
    return internal::MailboxData::Instance()->createMessage<Progress>(msg, cancelFunction);
}

Timer* CreateTimer(const std::string& msg, const std::function<void(void)>& cancelFunction) {
    return internal::MailboxData::Instance()->createMessage<Timer>(msg, cancelFunction);
}

void ShowMessages(void) {
    internal::MailboxData::Instance()->showMessages();
}

void Clear() {
    internal::MailboxData::Instance()->clear();
}

} // namespace mailbox
} // namespace GRender