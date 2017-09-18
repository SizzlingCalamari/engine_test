
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Mesh;

struct GLMesh
{
    GLMesh();
    GLMesh(const GLMesh& other) = delete;
    GLMesh(GLMesh&& other);
    GLMesh& operator=(GLMesh&& other);
    GLMesh& operator=(const GLMesh& other) = delete;
    ~GLMesh();

    void FreeBuffers();
    static GLMesh GLMeshFromMesh(const Mesh& mesh);

    glm::vec3 minAABB;
    glm::vec3 maxAABB;

    uint vertexBufferId = 0;
    uint uvBufferId     = 0;
    uint normalBufferId = 0;
    size_t numVertices  = 0;
};
