#pragma once

#include <glm/glm.hpp>
#include "eventsWrapper.h"

namespace GRender::mouse {

float Wheel();
glm::vec2 Position();
glm::vec2 Delta();

bool IsDown(MouseButton btn);
bool IsReleased(MouseButton btn);

bool IsClicked(MouseButton btn);
bool IsDoubleClicked(MouseButton btn);

} // namespace GRender::mouse

namespace GRender::keyboard {
// Keys
bool IsDown(Key key);
bool IsPressed(Key key);
bool IsReleased(Key key);

// Letters
bool IsDown(char key);
bool IsPressed(char key);
bool IsReleased(char key);

// Numbers
bool IsDown(int32_t key);
bool IsPressed(int32_t key);
bool IsReleased(int32_t key);

} // namespace GRender::keyboard
