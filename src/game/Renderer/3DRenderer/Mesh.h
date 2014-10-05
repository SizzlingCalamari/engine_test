
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

struct Mesh
{
    Mesh():
        vertexBufferId(0),
        uvBufferId(0),
        normalBufferId(0),
        numVertices(0)
    {
    }

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvcoords;
    std::vector<glm::vec3> normals;
    glm::vec3 minAABB;
    glm::vec3 maxAABB;

    uint vertexBufferId;
    uint uvBufferId;
    uint normalBufferId;
    size_t numVertices;
};

uint LoadVertices(const void* vertices, ptrdiff_t stride, size_t count);
void FreeMesh(Mesh* mesh);
