#include "quad.h"

namespace GRender
{

    Quad::Quad(uint32_t numQuads) : maxVertices(4 * numQuads)
    {

        glad_glGenVertexArrays(1, &vao);
        glad_glBindVertexArray(vao);

        // Copying buffer to gpu
        glad_glGenBuffers(1, &vertex_buffer);
        glad_glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glad_glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        // layout for buffer
        glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
        glad_glEnableVertexAttribArray(0);

        glad_glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
        glad_glEnableVertexAttribArray(1);

        glad_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texCoord));
        glad_glEnableVertexAttribArray(2);

        glad_glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texID));
        glad_glEnableVertexAttribArray(3);

        // Preparing index array
        vIndex.resize(6 * uint64_t(numQuads));
        for (uint32_t k = 0; k < numQuads; k++)
        {
            vIndex[6 * uint64_t(k) + 0] = 4 * k + 0;
            vIndex[6 * uint64_t(k) + 1] = 4 * k + 1;
            vIndex[6 * uint64_t(k) + 2] = 4 * k + 2;
            vIndex[6 * uint64_t(k) + 3] = 4 * k + 0;
            vIndex[6 * uint64_t(k) + 4] = 4 * k + 2;
            vIndex[6 * uint64_t(k) + 5] = 4 * k + 3;
        }

        glad_glGenBuffers(1, &index_buffer);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndex.size() * sizeof(uint32_t), vIndex.data(), GL_STATIC_DRAW);

        // Allocating memory for vertex buffer
        vtxBuffer.resize(maxVertices);
    }

    Quad::~Quad(void)
    {
        glad_glDeleteBuffers(1, &index_buffer);
        glad_glDeleteBuffers(1, &vertex_buffer);
        glad_glDeleteVertexArrays(1, &vao);
    }

    void Quad::draw(const glm::vec3 &position, const glm::vec2 &size, float angle, const glm::vec4 &color, float texID, const glm::vec2 &coord0, const glm::vec2 &coord1)
    {
        assert(counter < maxVertices); // To make sure that we didn't exceed the maximum size

        glm::vec4 pos[] = {{-0.5f, -0.5f, 0.0f, 1.0f},
                           {+0.5f, -0.5f, 0.0f, 1.0f},
                           {+0.5f, +0.5f, 0.0f, 1.0f},
                           {-0.5f, +0.5f, 0.0f, 1.0f}};

        glm::vec2 tCoord[] = {{coord0.x, coord0.y},
                              {coord1.x, coord0.y},
                              {coord1.x, coord1.y},
                              {coord0.x, coord1.y}};

        glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotate = glm::rotate(translate, angle, {0.0f, 0.0f, 1.0f});
        glm::mat4 scale = glm::scale(rotate, {size.x, size.y, 1.0f});

        for (uint32_t k = 0; k < 4; k++)
        {
            glm::vec4 vec = scale * pos[k];
            vtxBuffer[counter++] = {.pos = {vec.x, vec.y, vec.z}, .color = color, .texCoord = tCoord[k], .texID = texID};
        }
    }

    void Quad::draw(const glm::vec3 &position, const glm::vec2 &size, float angle, float texID, const glm::vec2 &coord0, const glm::vec2 &coord1)
    {
        draw(position, size, angle, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texID, coord0, coord1);
    }

    void Quad::submit(void)
    {
        // Binding buffers
        glad_glBindVertexArray(vao);

        // Let's send our data to the GPU
        glad_glBufferSubData(GL_ARRAY_BUFFER, 0, counter * sizeof(Vertex), vtxBuffer.data());

        // Issueing the draw call
        glad_glDrawElements(GL_TRIANGLES, 6 * (counter >> 2), GL_UNSIGNED_INT, 0);

        // Unbiding buffer
        glad_glBindVertexArray(0);

        // Resetting counter to zero for next round
        counter = 0;

    } // drawObject

}
