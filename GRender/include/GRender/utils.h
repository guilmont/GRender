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


static void TextSpaceAndWidth(std::string& txt, float split, float width) {
    ImGui::Text(txt.c_str());
	txt = "##" + txt;

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

// Drag width for normal primitive variables
template <typename TP>
static bool Drag(std::string label, TP& value,
          const float split = -1.0f,  // 0 < val < 1 -> ratio of available space to start box
          const float boxWidth = -1.0f, // 0 < val < 1 -> ratio of space for box width
          const float speed = 1.0f,
          const TP minValue = std::numeric_limits<TP>::min(),
          const TP maxValue = std::numeric_limits<TP>::max(),
          const char* format = nullptr) {

    internal::TextSpaceAndWidth(label, split, boxWidth);
    auto [dtype, fmt] = internal::getParameters<TP>();

    return ImGui::DragScalar(label.c_str(), dtype, &value, speed, &minValue, &maxValue,
                             format ? format : fmt);
}

// Drag widget for glm vectors of any size
template <typename TP, size_t N>
static bool Drag(std::string label, glm::vec<N, TP>& value,
          const float split = -1.0f,
          const float boxWidth = -1.0f,
          const float speed = 1.0f,
          const TP minValue = std::numeric_limits<TP>::min(),
          const TP maxValue = std::numeric_limits<TP>::max(),
          const char* format = nullptr) {

    internal::TextSpaceAndWidth(label, split, boxWidth);
    auto [dtype, fmt] = internal::getParameters<TP>();

    return ImGui::DragScalarN(label.c_str(), dtype, glm::value_ptr(value), N, speed,
                              &minValue, &maxValue, format ? format : fmt);
}

static bool RGB_Edit(std::string label, glm::vec3& color, float split = -1.0f, float boxWidth = -1.0f) {
    internal::TextSpaceAndWidth(label, split, boxWidth);
    return ImGui::ColorEdit3(label.c_str(), glm::value_ptr(color));
}

static bool RGBA_Edit(std::string label, glm::vec4& color, float split = -1.0f, float boxWidth = -1.0f) {
    internal::TextSpaceAndWidth(label, split, boxWidth);
    return ImGui::ColorEdit4(label.c_str(), glm::value_ptr(color));
}

static bool Checkbox(std::string label, bool& check, float split = -1.0f) {
    internal::TextSpaceAndWidth(label, split, -1.0f);
    return ImGui::Checkbox(label.c_str(), &check);
} 

} // namespace utils

