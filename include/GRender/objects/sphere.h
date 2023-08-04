#pragma once

#include "GRender/objects/object.h"

namespace GRender {

struct Sphere : public Object {
    Sphere(void) = default;
    Sphere(const uint32_t maxNumSpheres);
    ~Sphere(void) = default;

    Sphere(const Sphere&) noexcept = delete;
    Sphere& operator=(const Sphere&) noexcept = delete;

    Sphere(Sphere&&) noexcept;
    Sphere& operator=(Sphere&&) noexcept;
};

} // namespace GRender
