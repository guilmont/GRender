#include "events.h"

namespace GRender {

///////////////////////////////////////////////////////////
// MOUSE

float Mouse::wheel() const {
    return ImGui::GetIO().MouseWheel;
}

glm::vec2 Mouse::position() const {
    ImGuiIO& io = ImGui::GetIO();
    return { io.MousePos.x, io.MousePos.y };
}

glm::vec2 Mouse::delta() const {
    ImGuiIO& io = ImGui::GetIO();
    return { io.MouseDelta.x, io.MouseDelta.y };
}

bool Mouse::isPressed(MouseButton btn) const {
    return ImGui::IsMouseDown(static_cast<ImGuiMouseButton>(btn));
}

bool Mouse::isClicked(MouseButton btn) const {
    return ImGui::IsMouseClicked(static_cast<ImGuiMouseButton>(btn));
}

bool Mouse::isDoubleClicked(MouseButton btn) const {
    return ImGui::IsMouseDoubleClicked(static_cast<ImGuiMouseButton>(btn));
}

bool Mouse::isReleased(MouseButton btn) const {
    return ImGui::IsMouseReleased(static_cast<ImGuiMouseButton>(btn));
}

///////////////////////////////////////////////////////////
// KEYBOARD

bool Keyboard::isDown(Key key) const {
    return ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
}

bool Keyboard::isPressed(Key key) const {
    return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
}

bool Keyboard::isReleased(Key key) const {
    return ImGui::IsKeyReleased(static_cast<ImGuiKey>(key));
}

bool Keyboard::isDown(char key) const {
    ImGuiKey foo = convertLetter(key);
    return ImGui::IsKeyDown(foo);
}

bool Keyboard::isPressed(char key) const {
    ImGuiKey foo = convertLetter(key);
    return ImGui::IsKeyPressed(foo);
}

bool Keyboard::isReleased(char key) const {
    ImGuiKey foo = convertLetter(key);
    return ImGui::IsKeyReleased(foo);
}

bool Keyboard::isDown(int32_t key) const {
    ImGuiKey foo = convertNumber(key);
    return ImGui::IsKeyDown(foo);
}

bool Keyboard::isPressed(int32_t key) const {
    ImGuiKey foo = convertNumber(key);
    return ImGui::IsKeyPressed(foo);
}

bool Keyboard::isReleased(int32_t key) const {
    ImGuiKey foo = convertNumber(key);
    return ImGui::IsKeyReleased(foo);
}

ImGuiKey Keyboard::convertLetter(char key) const {
    return key - 'A' + ImGuiKey_A;
}

ImGuiKey Keyboard::convertNumber(int32_t key) const {
    return key + ImGuiKey_0;
}

} // namespace GRender
