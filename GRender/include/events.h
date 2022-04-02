#pragma once

#include <glm/glm.hpp>
#include "eventsWrapper.h"

namespace GRender {

struct Mouse {
    Mouse() = default;
    ~Mouse() = default;

    float wheel() const;
    glm::vec2 position() const;
    glm::vec2 delta() const;
        
    bool isPressed(MouseButton btn) const;
    bool isReleased(MouseButton btn) const;

    bool isClicked(MouseButton btn) const;
    bool isDoubleClicked(MouseButton btn) const;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class Keyboard {
public:
    Keyboard() = default;
    ~Keyboard() = default;

    // Keys
    bool isDown(Key key) const;
    bool isPressed(Key key) const;
    bool isReleased(Key key) const;

    // Letters
    bool isDown(char key) const;
    bool isPressed(char key) const;
    bool isReleased(char key) const;

    // Numbers
    bool isDown(int32_t key) const;
    bool isPressed(int32_t key) const;
    bool isReleased(int32_t key) const;

private:
    ImGuiKey convertLetter(char key) const;
    ImGuiKey convertNumber(int32_t key) const;
};

} // namespace GRender
