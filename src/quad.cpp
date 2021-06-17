#include "quad.h"

namespace GRender
{

    Quad::Quad(void)
    {

        vIndex = { 0, 1, 2, 0, 2, 3 };

        vtxBuffer = {
            {{-0.5f, -0.5f, 0.0}, {0.0f, 0.0f}},
            {{+0.5f, -0.5f, 0.0}, {1.0f, 0.0f}},
            {{+0.5f, +0.5f, 0.0}, {1.0f, 1.0f}},
            {{-0.5f, +0.5f, 0.0}, {0.0f, 1.0f}},
        };

        glad_glGenVertexArrays(1, &vao);
        glad_glBindVertexArray(vao);

        // Copying buffer to gpu
        glad_glGenBuffers(1, &vertex_buffer);
        glad_glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glad_glBufferData(GL_ARRAY_BUFFER, vtxBuffer.size() * sizeof(Vertex), vtxBuffer.data(), GL_DYNAMIC_DRAW);

        // layout for buffer
        glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glad_glEnableVertexAttribArray(0);

        glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glad_glEnableVertexAttribArray(1);

        // submiting index array
        glad_glGenBuffers(1, &index_buffer);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndex.size() * sizeof(uint32_t), vIndex.data(), GL_STATIC_DRAW);

    } 

    Quad::~Quad(void)
    {
        glad_glDeleteBuffers(1, &index_buffer);
        glad_glDeleteBuffers(1, &vertex_buffer);
        glad_glDeleteVertexArrays(1, &vao);
    } // destructor

    void Quad::draw(void)
    {
        glad_glBindVertexArray(vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glad_glDrawElements(GL_TRIANGLES, int(vIndex.size()), GL_UNSIGNED_INT, 0);
        glad_glBindVertexArray(0);
    } // drawObject


}