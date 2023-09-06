#pragma once

#include "core.h"

namespace GRender::utils::internal {

// Drag widget
template<typename TP>
inline std::pair<ImGuiDataType, const char*> getParameters() {
    ASSERT(false, "Type not supported!!");
    return {0, nullptr};
}

template<>
inline std::pair<ImGuiDataType, const char*> getParameters<int32_t>() { return {ImGuiDataType_S32, "%d"}; }
template<>
inline std::pair<ImGuiDataType, const char*> getParameters<uint32_t>() { return {ImGuiDataType_U32, "%d"}; }
template<>
inline std::pair<ImGuiDataType, const char*> getParameters<size_t>() { return {ImGuiDataType_U64, "%ld"}; }
template<>
inline std::pair<ImGuiDataType, const char*> getParameters<float>() { return {ImGuiDataType_Float, "%.3f"}; }
template<>
inline std::pair<ImGuiDataType, const char*> getParameters<double>() { return {ImGuiDataType_Double, "%.3f"}; }


static void TextSpaceAndWidth(const std::string& txt, float split, float width) {
    ImGui::Text("%s", txt.c_str());

    if (split < 0.0f) { ImGui::SameLine(); }
    else              { ImGui::SameLine(split * ImGui::GetContentRegionAvail().x); }

    if (width > 0.0f ) { ImGui::SetNextItemWidth(0.98f * width * ImGui::GetContentRegionAvail().x); }
}

} // namespace GRender::utils::internal

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace GRender::utils {

// Measuring performance
void PerformanceDisplay(bool& view);

// Show ImGui Demo with all widgets and more
void ViewWidgetsDemo(bool& view);

/// DRAG //////////////////////////////////////////////////////////////////////

template <typename TP>
static bool Drag(std::string label, TP& value,
    const float split = -1.0f,  // 0 < val < 1 -> ratio of available space to start box
    const float boxWidth = -1.0f, // 0 < val < 1 -> ratio of space for box width
    const float speed = 1.0f,
    const TP minValue = std::numeric_limits<TP>::min(),
    const TP maxValue = std::numeric_limits<TP>::max(),
    const char* format = nullptr,
    const float power = 1.0f) {

    internal::TextSpaceAndWidth(label, split, boxWidth);
    auto [imType, fmt] = internal::getParameters<TP>();

    return ImGui::DragScalar(label.c_str(), imType, &value, speed, &minValue, &maxValue, format ? format : fmt, power);
}

template <typename TP, size_t N>
static bool Drag(const std::string& label, glm::vec<N, TP>& value,
    const float split = -1.0f,
    const float boxWidth = -1.0f,
    const float speed = 1.0f,
    const TP minValue = std::numeric_limits<TP>::min(),
    const TP maxValue = std::numeric_limits<TP>::max(),
    const char* format = nullptr,
    const float power = 1.0f) {

    internal::TextSpaceAndWidth(label, split, boxWidth);
    auto [imType, fmt] = internal::getParameters<TP>();
    return ImGui::DragScalarN(("##"+label).c_str(), imType, glm::value_ptr(value), N, speed, &minValue, &maxValue, format ? format : fmt, power);
}

/// SLIDERS ///////////////////////////////////////////////////////////////////

template <typename TP>
static bool Slider(std::string label, TP& value,
    const float split = -1.0f,  // 0 < val < 1 -> ratio of available space to start box
    const float boxWidth = -1.0f, // 0 < val < 1 -> ratio of space for box width
    const float speed = 1.0f,
    const TP minValue = std::numeric_limits<TP>::min(),
    const TP maxValue = std::numeric_limits<TP>::max(),
    const char* format = nullptr,
    const float power = 1.0f) {

    internal::TextSpaceAndWidth(label, split, boxWidth);
    auto [imType, fmt] = internal::getParameters<TP>();
    return ImGui::SliderScalar(label.c_str(), imType, &value, speed, &minValue, &maxValue, format ? format : fmt, power);
}

template <typename TP, size_t N>
static bool Slider(const std::string& label, glm::vec<N, TP>& value,
    const float split = -1.0f,
    const float boxWidth = -1.0f,
    const float speed = 1.0f,
    const TP minValue = std::numeric_limits<TP>::min(),
    const TP maxValue = std::numeric_limits<TP>::max(),
    const char* format = nullptr,
    const float power = 1.0f) {

    internal::TextSpaceAndWidth(label, split, boxWidth);
    auto [imType, fmt] = internal::getParameters<TP>();
    return ImGui::DragScalarN(("##"+label).c_str(), imType, glm::value_ptr(value), N, speed, &minValue, &maxValue, format ? format : fmt, power);
}

/// COLORS ////////////////////////////////////////////////////////////////////

static inline bool RGB_Edit(const std::string& label, glm::vec3& color, float split = -1.0f, float boxWidth = -1.0f) {
    internal::TextSpaceAndWidth(label, split, boxWidth);
    return ImGui::ColorEdit3(("##"+label).c_str(), glm::value_ptr(color));
}

static inline bool RGBA_Edit(const std::string& label, glm::vec4& color, float split = -1.0f, float boxWidth = -1.0f) {
    internal::TextSpaceAndWidth(label, split, boxWidth);
    return ImGui::ColorEdit4(("##"+label).c_str(), glm::value_ptr(color));
}

/// OTHER /////////////////////////////////////////////////////////////////////
static inline bool Checkbox(const std::string& label, bool& check, float split = -1.0f) {
    internal::TextSpaceAndWidth(label, split, -1.0f);
    return ImGui::Checkbox(("##"+label).c_str(), &check);
}

static inline bool InputText(const std::string& label, std::string& buffer,
                      float split = -1.0f, float boxWidth = -1.0f,
                       ImGuiInputTextFlags flags = ImGuiInputTextFlags_None,
                       ImGuiInputTextCallback callback = nullptr, void* userData = nullptr) {
    internal::TextSpaceAndWidth(label, split, boxWidth);
    return ImGui::InputText(("##" + label).c_str(), buffer.data(), buffer.size(), flags, callback, userData);
}

} // namespace utils

