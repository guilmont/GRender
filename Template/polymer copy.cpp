#include "polymer.h"

#define PI 3.14159265358979323846f

Polymer::Polymer(float kuhn, uint64_t numBeads, uint64_t divisions) : numBeads(numBeads), divisions(divisions)
{

    // Let's create small templates for the spheres and cylinders
    uint64_t 
        nvtxCylinder = 2 * divisions,   // Top and bottom are not supposed to  be seem
        nidCylinder = 6 * divisions;

    cylinder.id.resize(nidCylinder);

    float
        dtheta = 2.0f * PI / float(divisions),
        dphi = PI / float(divisions-1);


    /////////////////////////////////////////////////////////////////////////////////////
    // Creating vertices for sphere
    // TODO: Remove excess vertices at top and bottom
    uint64_t
        nvtxSphere = divisions * divisions,
        nidSphere = 6 * divisions * (divisions-1);

    sphere.vtx.resize(nvtxSphere);
    sphere.id.resize(nidSphere);

    sphereRadius = 0.5f * kuhn;
    cylinderRadius = 0.3f * sphereRadius;

    uint64_t num = 0;
    for (uint64_t ctp = 0; ctp < divisions; ctp++)
        for (uint64_t ctt = 0; ctt < divisions; ctt++)
        {
            float
                phi = float(ctp) * dphi,
                theta = float(ctt) * dtheta;

            float
                x = sin(phi) * cos(theta),
                y = cos(phi),
                z = sin(phi) * sin(theta);

            sphere.vtx.at(num++) = { { x, y, z }, { x, y, z }, sphereColor };
        }


    // Now we create the index array for the sphere
    num = 0;
    for (uint64_t r = 0; r < divisions-1; r++)
        for (uint64_t k = 0; k < divisions; k++)
        {
            uint64_t l = (k + 1) % divisions;

            sphere.id.at(num++) = r * divisions + k;
            sphere.id.at(num++) = (r + 1) * divisions + k;
            sphere.id.at(num++) = (r + 1) * divisions + l;

            sphere.id.at(num++) = r * divisions + k;
            sphere.id.at(num++) = (r + 1) * divisions + l;
            sphere.id.at(num++) = r * divisions + l;
        }

    /////////////////////////////////////////////////////////////////////////////////////
    // We are going to create vertices emplace as needed upon update
    // Now we create the index array for the sphere

    num = 0;
    for (uint64_t k = 0; k < divisions; k++)
    {
        uint64_t l = (k + 1) % divisions;

        cylinder.id.at(num++) = k;
        cylinder.id.at(num++) = divisions + k;
        cylinder.id.at(num++) = divisions + l;

        cylinder.id.at(num++) = k;
        cylinder.id.at(num++) = divisions + l;
        cylinder.id.at(num++) = l;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

     glGenVertexArrays(1, &vao);
     glBindVertexArray(vao);

     // Copying buffer to gpu
     maxVertices = numBeads * nvtxSphere + (numBeads - 1) * nvtxCylinder;
     maxIndices = numBeads * nidSphere + (numBeads - 1) * nidCylinder;

     glGenBuffers(1, &vertex_buffer);
     glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
     glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

     // layout for buffer
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
     glEnableVertexAttribArray(0);

     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
     glEnableVertexAttribArray(1);

     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
     glEnableVertexAttribArray(2);

     // setup index buffer
     glGenBuffers(1, &index_buffer);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndices * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);
     
}

Polymer::~Polymer(void)
{
    if (vao> 0)
    {
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteVertexArrays(1, &vao);
        
        index_buffer = vertex_buffer = vao = 0;
    }
}


void Polymer::update(const std::vector<glm::vec3>& positions)
{
    assert(positions.size() == numBeads);

    std::vector<Vertex> vtx(maxVertices);
    std::vector<uint32_t> idx(maxIndices);

    // Calculating sines and cosines to create cylinders later in a optimized fashion
    float dtheta = 2.0f * PI / float(divisions);

    std::vector<float> vCos(divisions), vSin(divisions);
    for (uint64_t k = 0; k < divisions; k++)
    {
        float ang = float(k) * dtheta;
        vCos.at(k) = cos(ang);
        vSin.at(k) = sin(ang);
    }


    // Setting up all vertices for the polymers
    uint64_t
        vtxCounter = 0,
        idCounter = 0;

    for (uint64_t bead = 0; bead < numBeads; bead++)
    {
        const glm::vec3& pos = positions[bead];

        // Inserting indices
        for (uint32_t val : sphere.id)
            idx[idCounter++] = vtxCounter + val;

        // Copying unitary sphere into position
        for (Vertex var : sphere.vtx)
        {
            // normals and color are already set
            var.pos.x = sphereRadius * var.pos.x + pos.x;
            var.pos.y = sphereRadius * var.pos.y + pos.y;
            var.pos.z = sphereRadius * var.pos.z + pos.z;
            vtx.at(vtxCounter++) = std::move(var);
        }


        //if ((bead + 1) == numBeads)
        //    continue;


        ///////////////////////
        //// Creating vertices for cylinder
        //const glm::vec3& pos_next = positions[bead + 1];


        //// Setting up indices for the cylinder
        //for (uint32_t val : cylinder.id)
        //    idx[idCounter++] = vtxCounter + val;

        //glm::vec3
        //    cm = 0.5f * (pos + pos_next),
        //    dir = pos_next - pos;

        //float len = 0.5f * glm::length(dir);
        //glm::vec3 r3 = glm::normalize(dir);
        //glm::vec3 r1 = glm::normalize(glm::vec3{ -(dir.y + dir.z + 2e-5f) / (dir.x + 1e-5), 1.0f, 1.0f });
        //glm::vec3 r2 = glm::cross(r1, r3);

        //for (int32_t i = 1; i >= -1; i-=2)
        //    for (uint32_t ag = 0; ag < divisions; ag++)
        //    {
        //        Vertex var;
        //        var.pos = cylinderRadius * (vCos[ag] * r1 + vSin[ag] * r2) + float(i) * len * r3 + cm;
        //        var.normal = glm::normalize(vSin[ag] * r1 + vCos[ag] * r2);
        //        var.color = cylinderColor;

        //        vtx.at(vtxCounter++) = std::move(var);
        //    }

    }


    // Binding buffers
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    // Let's send our data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vtxCounter * sizeof(Vertex), vtx.data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idCounter * sizeof(uint32_t), idx.data());

    // Unbiding vao
    glBindVertexArray(0);


    std::ofstream arq("C:/Users/GM Oliveira/Dropbox/Software/GRender/Template/vtx.txt");
    arq.precision(2);
    for (const Vertex& var : vtx)
        arq << std::fixed << var.pos.x << " " << var.pos.y << " " << var.pos.z << std::endl;

    arq.close();


    arq.open("C:/Users/GM Oliveira/Dropbox/Software/GRender/Template/index.txt");
    for (uint32_t k = 0; k < idx.size(); k+=3)
        arq << idx[k] << " " << idx[k+1] << " " << idx[k+2] << std::endl;

    arq.close();

}

void Polymer::submit(void)
{
    // Binding buffers
    glBindVertexArray(vao);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    
    // Issueing the draw call
    glPointSize(3.0f);
    glDrawElements(GL_TRIANGLES, maxIndices, GL_UNSIGNED_INT, nullptr);

    // Unbiding vao
    glBindVertexArray(0);
}

