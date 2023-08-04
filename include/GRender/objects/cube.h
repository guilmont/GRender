#pragma once

#include "GRender/objects/object.h"

namespace GRender {

struct Cube : public Object {
    Cube(void) = default;
    Cube(const uint32_t maxNumCubes);
    ~Cube(void) = default;

    Cube(const Cube&) noexcept = delete;
    Cube& operator=(const Cube&) noexcept = delete;

    Cube(Cube&&) noexcept;
    Cube& operator=(Cube&&) noexcept;
};

} // namespace GRender
