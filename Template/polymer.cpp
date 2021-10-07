#include "polymer.h"

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

Polymer::Polymer(uint32_t numBeads, uint32_t divisions) : numBeads(numBeads)
{

   createSphere(divisions);
   createCylinder(divisions);
     
}

Polymer::~Polymer(void)
{
    glDeleteBuffers(1, &sphere.vtx);
    glDeleteBuffers(1, &sphere.idx);
    glDeleteBuffers(1, &sphere.com);
    glDeleteVertexArrays(1, &sphere.vao);

    glDeleteBuffers(1, &cylinder.vtx);
    glDeleteBuffers(1, &cylinder.idx);
    glDeleteBuffers(1, &cylinder.com);
    glDeleteBuffers(1, &cylinder.dir);
    glDeleteVertexArrays(1, &cylinder.vao);

}


void Polymer::createSphere(uint32_t divisions)
{

    // Creating vertices
    float dtheta = PI / float(divisions),
          dphi = 2.0f * PI / float(divisions);

        
    sphere.vtxBuffer.emplace_back(glm::vec3{0.0, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, sphereColor); // top vertex

    for (uint64_t ctt = 1; ctt < divisions-1; ctt++)
        for (uint64_t ctp = 0; ctp < divisions; ctp++) 
        {
            float
                phi = float(ctp) * dphi,
                theta = float(ctt) * dtheta;

            float
                x = sin(theta) * cos(phi),
                y = sin(theta) * sin(phi),
                z = cos(theta);

            sphere.vtxBuffer.emplace_back(glm::vec3{ x, y, z }, glm::vec3{ x, y, z }, sphereColor);
        }

    sphere.vtxBuffer.emplace_back(glm::vec3{0.0, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}, sphereColor); // bottom vertex

    /////////////////////////////////////////////////////////////////////////////////////

    // Creating indices matrix 
    std::vector<uint32_t> vec(divisions * divisions);
    for (uint32_t k = 0; k < divisions; k++)
        for (uint32_t l = 0; l < divisions; l++)
        {
            if (k == 0)
                vec[k * divisions + l] = 0;
            else if (k+1 == divisions)
                vec[k * divisions + l] = sphere.vtxBuffer.size() - 1;
            else
                vec[k*divisions + l] = (k-1) * divisions + l + 1;
        }

    // Creating index buffer
    for (uint32_t k = 0; k < divisions-1; k++)
        for (uint32_t l = 0; l < divisions; l++)
        {
            uint32_t r1, r2, r3;

            if (k > 0)
            {
                // top triangle
                r1 = k * divisions+l;
                r2 = r1 + divisions;
                r3 = k * divisions + ((l+1) % divisions);
                sphere.idxBuffer.emplace_back(vec[r1], vec[r2], vec[r3]);
            }
        
            // bottom triangle
            r1 = (k + 1) * divisions+l;
            r2 = (k + 1) * divisions + ((l+1) % divisions);
            r3 = k * divisions + ((l+1) % divisions);
            sphere.idxBuffer.emplace_back(vec[r1], vec[r2], vec[r3]);
        }
    

    /////////////////////////////////////////////////////////////////////////////////////

     glGenVertexArrays(1, &sphere.vao);
     glBindVertexArray(sphere.vao);

     glGenBuffers(1, &sphere.vtx);
     glBindBuffer(GL_ARRAY_BUFFER, sphere.vtx);
     glBufferData(GL_ARRAY_BUFFER, sphere.vtxBuffer.size() * sizeof(Vertex), sphere.vtxBuffer.data(), GL_DYNAMIC_DRAW);

     // layout for buffer
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
     glEnableVertexAttribArray(0);

     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
     glEnableVertexAttribArray(1);

     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
     glEnableVertexAttribArray(2);

     // instance buffer
     glGenBuffers(1, &sphere.com);
     glBindBuffer(GL_ARRAY_BUFFER, sphere.com);
     glBufferData(GL_ARRAY_BUFFER, numBeads * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

     glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
     glEnableVertexAttribArray(3);
     glVertexAttribDivisor(3, 1);

     // setup index buffer
     glGenBuffers(1, &sphere.idx);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.idx);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.idxBuffer.size() * sizeof(glm::uvec3), sphere.idxBuffer.data(), GL_STATIC_DRAW);
}

void Polymer::createCylinder(uint32_t divisions)
{
    // We are never going to see the ends of this cylinder, let's save from triangles from there
    float  dphi = 2.0f * PI / float(divisions);

    for (float x = -0.5f; x < 0.51f; x+=1.0f)
        for (uint32_t ct = 0; ct < divisions; ct++)
        {
            float phi = float(ct) * dphi;

            Vertex v;
            v.pos = {x, cos(phi), sin(phi)};
            v.normal = glm::normalize(glm::vec3{0.0, v.pos.y, v.pos.z});
            v.color = cylinderColor;

            cylinder.vtxBuffer.emplace_back(std::move(v));
        }


    // Setting up index buffer
    for (uint32_t k = 0; k < divisions; k++)
    {
        uint32_t l = (k+1) % divisions;

        // top triangle
        cylinder.idxBuffer.emplace_back(k, divisions + k, l);
        
        // bottom triangle
        cylinder.idxBuffer.emplace_back(divisions + k,  l, divisions + l);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    glGenVertexArrays(1, &cylinder.vao);
    glBindVertexArray(cylinder.vao);

    // Copying buffer to gpu
    glGenBuffers(1, &cylinder.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.vtx);
    glBufferData(GL_ARRAY_BUFFER, cylinder.vtxBuffer.size() * sizeof(Vertex), cylinder.vtxBuffer.data(), GL_DYNAMIC_DRAW);

    // layout for buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // instance center of mass buffer
    glGenBuffers(1, &cylinder.com);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.com);
    glBufferData(GL_ARRAY_BUFFER, (numBeads - 1)* sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // instance direction buffer
    glGenBuffers(1, &cylinder.dir);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.dir);
    glBufferData(GL_ARRAY_BUFFER, (numBeads - 1) * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);


    // setup index buffer
    glGenBuffers(1, &cylinder.idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder.idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder.idxBuffer.size() * sizeof(glm::uvec3), cylinder.idxBuffer.data(), GL_STATIC_DRAW);
}


void Polymer::update(const std::vector<glm::vec3>& positions)
{
    // Updating sphere positions
    glBindVertexArray(sphere.vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.com);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBeads * sizeof(glm::vec3), positions.data());

    // Updating connections
    std::vector<glm::vec3> com(numBeads - 1), dir(numBeads - 1);
    for (uint32_t k = 0; k < numBeads - 1; k++)
    {
        com.at(k) = 0.5f * (positions.at(k) + positions.at(k+1));
        dir.at(k) = positions.at(k+1) - positions.at(k);
    }

    glBindVertexArray(cylinder.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.com);
    glBufferSubData(GL_ARRAY_BUFFER, 0, com.size() * sizeof(glm::vec3), com.data());

    glBindBuffer(GL_ARRAY_BUFFER, cylinder.dir);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dir.size() * sizeof(glm::vec3), dir.data());
}


void Polymer::submitBlobs(void)
{
    glBindVertexArray(sphere.vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.com);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.idx);    
    glDrawElementsInstanced(GL_TRIANGLES, 3 * sphere.idxBuffer.size(), GL_UNSIGNED_INT, nullptr, numBeads);
}


void Polymer::submitConnections(void)
{
    glBindVertexArray(cylinder.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.com);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.dir);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder.idx);
    glDrawElementsInstanced(GL_TRIANGLES, 3 * cylinder.idxBuffer.size(), GL_UNSIGNED_INT, nullptr, numBeads-1);
}

void Polymer::updateSphereColor(void)
{
    for (Vertex& v : sphere.vtxBuffer)
        v.color = sphereColor;

    glBindVertexArray(sphere.vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.vtx);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere.vtxBuffer.size() * sizeof(Vertex), sphere.vtxBuffer.data());
}

void Polymer::updateCylinderColor(void)
{
    for (Vertex& v : cylinder.vtxBuffer)
        v.color = cylinderColor;

    glBindVertexArray(cylinder.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.vtx);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cylinder.vtxBuffer.size() * sizeof(Vertex), cylinder.vtxBuffer.data());
}