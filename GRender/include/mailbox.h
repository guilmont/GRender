#pragma once

#include <list>
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

///////////////////////////

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
    Progress(const std::string& msg, void (*cancelFunction)(void*), void* ptr = nullptr);
    void show(void) override;
    float progress = 0.0f;

private:
    void (*function)(void*);
    void* ptr = nullptr;

    TimePoint zero, current;
};

class Timer : public Message {
public:
    Timer(const std::string& msg, void (*cancelFunction)(void*), void* ptr = nullptr);
    void show(void) override;
    void stop(void);

private:
    void (*function)(void*);
    void* ptr = nullptr;

    TimePoint zero, current;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class Mailbox {
public:
    Mailbox(void) = default;
    ~Mailbox(void);

    Info* createInfo(const std::string& msg);
    Warn* createWarn(const std::string& msg);
    Error* createError(const std::string& msg);
    Progress* createProgress(const std::string& msg, void (*function)(void*), void *ptr = nullptr);
    Timer* createTimer(const std::string& msg, void (*function)(void*), void *ptr = nullptr);

    void open(void); // Schedules mailbox to display next iterations
    void close(void); // Hides in next iterations

    void showMessages(void);
    void clear(); // Cleans already read messages

private:
    bool active = false;
    std::list<Message *> messages;
};

} // namespace GRender
