
#include "Mesh.h"
#include "GL/glew.h"

uint LoadVertices(const void* vertices, ptrdiff_t stride, size_t num_vertices)
{
    uint bufferId = 0;
    glGenBuffers(1, &bufferId);
    assert(bufferId > 0);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, num_vertices*stride, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return bufferId;
}

void FreeMesh(Mesh* mesh)
{
    assert(mesh);
    uint vertexBufferId = mesh->vertexBufferId;
    uint normalBufferId = mesh->normalBufferId;
    uint uvBufferId = mesh->uvBufferId;
    if (vertexBufferId > 0)
    {
        glDeleteBuffers(1, &vertexBufferId);
    }
    if (normalBufferId > 0)
    {
        glDeleteBuffers(1, &vertexBufferId);
    }
    if (uvBufferId > 0)
    {
        glDeleteBuffers(1, &uvBufferId);
    }

    mesh->vertexBufferId = 0;
    mesh->normalBufferId = 0;
    mesh->uvBufferId = 0;
    mesh->numVertices = 0;
}
