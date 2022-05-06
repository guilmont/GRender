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

Progress::Progress(const std::string& msg, void (*cancelFunction)(void*), void* ptr) 
    : Message(msg), function(cancelFunction), ptr(ptr), zero(Clock::now()) {}

void Progress::show(void) {
    if (!is_read)
        current = Clock::now();

    ImGui::Text("%s :: Time: %s", content.c_str(), time2String(zero, current).c_str());

    float width = 0.8f * ImGui::GetContentRegionAvail().x;
    ImGui::ProgressBar(progress, { width, 0 }); // 0 goes for automatic height

    if (!is_read) {
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            is_read = true;
            function(ptr);
        }
    }

    ImGui::Spacing();
    if (progress >= 1.0f) {
        is_read = true;
        progress = 1.0f;
    }
}

/////////////////////////////

Timer::Timer(const std::string& msg, void (*cancelFunction)(void*), void* ptr)
    : Message(msg), function(cancelFunction), ptr(ptr), zero(Clock::now()) {}

void Timer::show(void) {
    if (!is_read)
        current = Clock::now();

    ImGui::Text("%s :: Time: %s", content.c_str(), time2String(zero, current).c_str());

    if (!is_read) {
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            stop();
            function(ptr);
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

struct MailboxData {
    bool active;
    std::list<Message*> messages;

    MailboxData(void) = default;
    ~MailboxData(void);

    template <typename TP, typename... Args>
    TP* createMessage(Args... args);

    void open(void);
    void close(void);

    void showMessages();
    void clear();

};

/////////////////////////////

MailboxData::~MailboxData() {
    for (Message* msg : messages) {
        delete msg;
    }
    messages.clear();
}

void MailboxData::open() {
    active = true;
}

void MailboxData::close() {
    active = false;
}

template <typename TP, typename... Args>
TP* MailboxData::createMessage(Args... args) {
    open();
    messages.emplace_back(new TP(args...));
    return reinterpret_cast<TP*>(messages.back());
}

void MailboxData::showMessages() {
    if (!active)
        return;

    ImGui::Begin("Mailbox", &active);
    const ImVec2 workpos = ImGui::GetMainViewport()->WorkPos;
    ImGui::SetWindowSize({ DPI_FACTOR * 720.0f, DPI_FACTOR * 405.0f });

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.02f, 0.02f, 0.02f, 1.0f });

    ImVec2 size = { 0.98f * ImGui::GetWindowWidth(), 0.82f * ImGui::GetWindowHeight() };
    ImGui::BeginChild("mail_child", size, true, ImGuiWindowFlags_HorizontalScrollbar);

    for (Message* msg : messages)
        msg->show();

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    if (ImGui::Button("Clear"))
        Clear();

    ImGui::SameLine();
    if (ImGui::Button("Close"))
        Close();

    ImGui::End();
}

void MailboxData::clear() {
    messages.remove_if(
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

// Global parameter in the GRender context
internal::MailboxData* mail = nullptr;

void Create() {
    mail = new internal::MailboxData();
}

void Destroy(void) {
    delete mail;
}

void Open() {
    mail->open();
}

void Close() {
    mail->close();
}

Info* CreateInfo(const std::string& msg) {
    return mail->createMessage<Info>(msg);
}

Warn* CreateWarn(const std::string& msg) {
    return mail->createMessage<Warn>(msg);
}

Error* CreateError(const std::string& msg) {
    return mail->createMessage<Error>(msg);
}

Progress* CreateProgress(const std::string& msg, void (*function)(void*), void* ptr) {
    return mail->createMessage<Progress>(msg, function, ptr);
}

Timer* CreateTimer(const std::string& msg, void(*function)(void*), void* ptr) {
    return mail->createMessage<Timer>(msg, function, ptr);
}

void ShowMessages(void) {
    ASSERT(mail, "Mailbox module was not created!");
    mail->showMessages();
}

void Clear() {
    mail->clear();
}

} // namespace mailbox
} // namespace GRender