#include "events.h"

namespace GRender::mouse {

float wheel() {
    return ImGui::GetIO().MouseWheel;
}

glm::vec2 position() {
    ImGuiIO& io = ImGui::GetIO();
    return { io.MousePos.x, io.MousePos.y };
}

glm::vec2 delta() {
    ImGuiIO& io = ImGui::GetIO();
    return { io.MouseDelta.x, io.MouseDelta.y };
}

bool isPressed(MouseButton btn) {
    return ImGui::IsMouseDown(static_cast<ImGuiMouseButton>(btn));
}

bool isClicked(MouseButton btn) {
    return ImGui::IsMouseClicked(static_cast<ImGuiMouseButton>(btn));
}

bool isDoubleClicked(MouseButton btn) {
    return ImGui::IsMouseDoubleClicked(static_cast<ImGuiMouseButton>(btn));
}

bool isReleased(MouseButton btn) {
    return ImGui::IsMouseReleased(static_cast<ImGuiMouseButton>(btn));
}

} // namespace GRender::mouse

/////////////////////////////////////////////////////////////////////////////////////////

namespace GRender::keyboard::internal {

ImGuiKey convertLetter(char key) {
    return key - 'A' + ImGuiKey_A;
}

ImGuiKey convertNumber(int32_t key) {
    return key + ImGuiKey_0;
}

} // namespace GRender::keyboard::internal

namespace GRender::keyboard {

bool isDown(Key key){
    return ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
}

bool isPressed(Key key){
    return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
}

bool isReleased(Key key){
    return ImGui::IsKeyReleased(static_cast<ImGuiKey>(key));
}

bool isDown(char key){
    ImGuiKey foo = internal::convertLetter(key);
    return ImGui::IsKeyDown(foo);
}

bool isPressed(char key){
    ImGuiKey foo = internal::convertLetter(key);
    return ImGui::IsKeyPressed(foo);
}

bool isReleased(char key){
    ImGuiKey foo = internal::convertLetter(key);
    return ImGui::IsKeyReleased(foo);
}

bool isDown(int32_t key){
    ImGuiKey foo = internal::convertNumber(key);
    return ImGui::IsKeyDown(foo);
}

bool isPressed(int32_t key){
    ImGuiKey foo = internal::convertNumber(key);
    return ImGui::IsKeyPressed(foo);
}

bool isReleased(int32_t key){
    ImGuiKey foo = internal::convertNumber(key);
    return ImGui::IsKeyReleased(foo);
}

} // namespace GRender::keyboard
