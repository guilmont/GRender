#pragma once

#include "core.h"
#include "eventsWrapper.h"

namespace GRender
{

    class  Event
    {
    public:
        Event(void) = default;
        virtual ~Event(void) = default;

        void set(const int32_t ev, int32_t action) { event[ev] = action; }

        int32_t operator[](const int32_t ev)
        {
            if (event.find(ev) == event.end())
                return -1;
            else
                return event[ev];
        }

        virtual void clear(void) = 0;
      

    protected:
        std::unordered_map<int32_t, int32_t> event;
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    struct  Mouse : public Event
    {
        glm::vec2 wheel = { 0.0f, 0.0f };
        glm::vec2 position = { 0.0f, 0.0f };
        glm::vec2 offset = { 0.0f, 0.0f };

        void clear(void) override
        {
            offset = wheel = { 0.0f, 0.0f };

            std::vector<int32_t> tags;
            for (auto& [tag, value] : event)
                if (value == GEvent::PRESS)
                    tags.push_back(tag);

            event.clear();
            for (int32_t& tag : tags)
                event[tag] = GEvent::PRESS;
        }

       
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    struct  Keyboard : public Event
    {

        void clear(void) override
        {

            std::vector<int32_t> tags;
            for (auto& [tag, value] : event)
                if (value == GEvent::PRESS || value == GEvent::REPEAT)
                    tags.push_back(tag);

            event.clear();
            for (int32_t& tag : tags)
                event[tag] = GEvent::PRESS;
        }
    };

}
