
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

struct Mesh
{
    std::vector<glm::vec3> verticies;
    std::vector<glm::vec2> uvcoords;
    std::vector<glm::vec3> normals;

    uint vertexBufferId = 0;
    uint uvBufferId = 0;
    uint normalBufferId = 0;
    size_t numVerticies = 0;
};

uint LoadVerticies(const void* verticies, ptrdiff_t stride, size_t count);
void FreeMesh(Mesh* mesh);
