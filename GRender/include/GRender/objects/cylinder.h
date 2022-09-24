#pragma once

#include "GRender/objects/object.h"

namespace GRender {

struct Cylinder : public Object {
    Cylinder(void) = default;
    Cylinder(const uint32_t maxNumCylinders);
    ~Cylinder(void) = default;

    Cylinder(const Cylinder&) noexcept = delete;
    Cylinder& operator=(const Cylinder&) noexcept = delete;

    Cylinder(Cylinder&&) noexcept;
    Cylinder& operator=(Cylinder&&) noexcept;

    // Utility function to calculate angles to rotate cylinder into a certain direction
    static glm::vec3 calcAnglesFromDirection(const glm::vec3& direction);
};

} // namespace GRender
