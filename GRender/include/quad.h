#pragma once

#include <vector>
#include "core.h"

namespace GRender
{
    class Quad
    {

    public:
        Quad(uint32_t numQuads);
        ~Quad(void);

        void draw(const glm::vec3& position, const glm::vec2& size, float angle, const glm::vec4& color, float texID, const glm::vec2& coord0 = { 0.0f, 0.0f }, const glm::vec2& coord1 = { 1.0f, 1.0f });
        void draw(const glm::vec3& position, const glm::vec2& size, float angle, float texID, const glm::vec2& coord0 = { 0.0f, 0.0f }, const glm::vec2& coord1 = { 1.0f, 1.0f });
        void draw(const glm::vec3& position, const glm::vec2& size, float angle, const glm::vec4& color);

        void submit(void);

    private:
    private:
        struct Vertex
        {
            glm::vec3 pos;
            glm::vec4 color;
            glm::vec2 texCoord;
            float texID;
        };

        uint32_t
            vao, // Vertex array object
            vertex_buffer,
            index_buffer;

        uint32_t
            counter = 0, // Current number of quads
            maxVertices = 0; // Maximum number of quads

        std::vector<uint32_t> vIndex;
        std::vector<Vertex> vtxBuffer;

    }; // class-object
}