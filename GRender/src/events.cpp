#include "events.h"

namespace GRender::mouse {

float Wheel() {
    return ImGui::GetIO().MouseWheel;
}

glm::vec2 Position() {
    ImGuiIO& io = ImGui::GetIO();
    return { io.MousePos.x, io.MousePos.y };
}

glm::vec2 Delta() {
    ImGuiIO& io = ImGui::GetIO();
    return { io.MouseDelta.x, io.MouseDelta.y };
}

bool IsDown(MouseButton btn) {
    return ImGui::IsMouseDown(static_cast<ImGuiMouseButton>(btn));
}

bool IsClicked(MouseButton btn) {
    return ImGui::IsMouseClicked(static_cast<ImGuiMouseButton>(btn));
}

bool IsDoubleClicked(MouseButton btn) {
    return ImGui::IsMouseDoubleClicked(static_cast<ImGuiMouseButton>(btn));
}

bool IsReleased(MouseButton btn) {
    return ImGui::IsMouseReleased(static_cast<ImGuiMouseButton>(btn));
}

} // namespace GRender::mouse

/////////////////////////////////////////////////////////////////////////////////////////

namespace GRender::keyboard::internal {

ImGuiKey convertLetter(char key) {
    return static_cast<ImGuiKey>(key);
}

ImGuiKey convertNumber(int32_t key) {
    key += static_cast<int32_t>('0');
    return static_cast<ImGuiKey>(key);
}

} // namespace GRender::keyboard::internal

namespace GRender::keyboard {

bool IsDown(Key key){
    return ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
}

bool IsPressed(Key key){
    return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
}

bool IsReleased(Key key){
    return ImGui::IsKeyReleased(static_cast<ImGuiKey>(key));
}

bool IsDown(char key){
    ImGuiKey foo = internal::convertLetter(key);
    return ImGui::IsKeyDown(foo);
}

bool IsPressed(char key){
    ImGuiKey foo = internal::convertLetter(key);
    return ImGui::IsKeyPressed(foo);
}

bool IsReleased(char key){
    ImGuiKey foo = internal::convertLetter(key);
    return ImGui::IsKeyReleased(foo);
}

bool IsDown(int32_t key){
    ImGuiKey foo = internal::convertNumber(key);
    return ImGui::IsKeyDown(foo);
}

bool IsPressed(int32_t key){
    ImGuiKey foo = internal::convertNumber(key);
    return ImGui::IsKeyPressed(foo);
}

bool IsReleased(int32_t key){
    ImGuiKey foo = internal::convertNumber(key);
    return ImGui::IsKeyReleased(foo);
}

} // namespace GRender::keyboard
