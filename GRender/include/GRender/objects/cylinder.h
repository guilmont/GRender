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
};

} // namespace GRender
