#pragma once

#include <glm/glm.hpp>
#include "eventsWrapper.h"

namespace GRender::mouse {

float wheel();
glm::vec2 position();
glm::vec2 delta();

bool isPressed(MouseButton btn);
bool isReleased(MouseButton btn);

bool isClicked(MouseButton btn);
bool isDoubleClicked(MouseButton btn);

} // namespace GRender::mouse

namespace GRender::keyboard {
// Keys
bool isDown(Key key);
bool isPressed(Key key);
bool isReleased(Key key);

// Letters
bool isDown(char key);
bool isPressed(char key);
bool isReleased(char key);

// Numbers
bool isDown(int32_t key);
bool isPressed(int32_t key);
bool isReleased(int32_t key);

} // namespace GRender::keyboard
