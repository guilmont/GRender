#pragma once

#include "core.h"

namespace GRender {

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

class Message {
public:
    Message(const std::string& msg) : content(msg) {}
    virtual ~Message(void) = default;
    virtual void show(void) = 0;

    bool is_read = false;

protected:
    std::string content;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

struct Info : public Message {
    Info(const std::string& msg);
    void show(void) override;
};

struct Warn : public Message {
    Warn(const std::string& msg);
    void show(void) override;
};

struct Error : public Message {
    Error(const std::string& msg);
    void show(void) override;
};

class Progress : public Message {
public:
    Progress(const std::string& msg, const std::function<void(void)>& cancelFunction = nullptr);
    void show(void) override;
    float progress = 0.0f;

private:
    std::function<void(void)> m_CancelFunction = nullptr;
    TimePoint zero, current;
};

class Timer : public Message {
public:
    Timer(const std::string& msg, const std::function<void(void)>& cancelFunction = nullptr);
    void show(void) override;
    void stop(void);

private:
    std::function<void(void)> m_CancelFunction = nullptr;
    TimePoint zero, current;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

namespace mailbox {

// Displays a message in the mailbox
Info* CreateInfo(const std::string& msg);
// Displays a warning message in the mailbox
Warn* CreateWarn(const std::string& msg);
// Displays an error message in the mailbox
Error* CreateError(const std::string& msg);
// Creates and displays a progress bar with cancel options in the mailbox
Progress* CreateProgress(const std::string& msg, const std::function<void(void)>& cancelFunction = nullptr);
// Creates and displays a timer with cancel options=
Timer* CreateTimer(const std::string& msg, const std::function<void(void)>& cancelFunction = nullptr);

// Schedules mailbox to display next iterations
void Open(void);
// Hides in next iterations
void Close(void);          
// Creates a windows to display all the messages
void ShowMessages(void);

// Destroys read messages
void Clear();

} // namespace mailbox
} // namespace GRender
