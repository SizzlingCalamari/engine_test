
#include "GLMesh.h"
#include "Mesh.h"
#include "GL/glew.h"

#include <assert.h>

GLMesh::GLMesh() = default;

GLMesh::GLMesh(GLMesh&& other):
    minAABB(other.minAABB),
    maxAABB(other.maxAABB),
    vertexBufferId(other.vertexBufferId),
    uvBufferId(other.uvBufferId),
    normalBufferId(other.normalBufferId),
    numVertices(other.numVertices)
{
    other.minAABB = glm::vec3(0.0f);
    other.maxAABB = glm::vec3(0.0f);
    other.vertexBufferId = 0;
    other.uvBufferId = 0;
    other.normalBufferId = 0;
    other.numVertices = 0;
}

GLMesh& GLMesh::operator=(GLMesh&& other)
{
    if((vertexBufferId > 0) || (normalBufferId > 0) || (uvBufferId > 0))
    {
        const GLuint bufferIds[] =
        {
            vertexBufferId,
            normalBufferId,
            uvBufferId,
        };
        glDeleteBuffers(3, bufferIds);
    }

    minAABB = other.minAABB;
    maxAABB = other.maxAABB;
    vertexBufferId = other.vertexBufferId;
    uvBufferId = other.uvBufferId;
    normalBufferId = other.normalBufferId;
    numVertices = other.numVertices;

    other.minAABB = glm::vec3(0.0f);
    other.maxAABB = glm::vec3(0.0f);
    other.vertexBufferId = 0;
    other.uvBufferId = 0;
    other.normalBufferId = 0;
    other.numVertices = 0;

    return *this;
}

GLMesh::~GLMesh()
{
    assert(vertexBufferId == 0);
    assert(uvBufferId == 0);
    assert(normalBufferId == 0);
}

void GLMesh::FreeBuffers()
{
    if((vertexBufferId > 0) || (normalBufferId > 0) || (uvBufferId > 0))
    {
        const GLuint bufferIds[] =
        {
            vertexBufferId,
            normalBufferId,
            uvBufferId,
        };
        glDeleteBuffers(3, bufferIds);

        vertexBufferId = 0;
        normalBufferId = 0;
        uvBufferId = 0;
    }

    numVertices = 0;
}

GLMesh GLMesh::GLMeshFromMesh(const Mesh& mesh)
{
    GLMesh glMesh;
    glMesh.minAABB = mesh.minAABB;
    glMesh.maxAABB = mesh.maxAABB;

    const size_t numVertices = mesh.vertices.size();
    const size_t numUvCoords = mesh.uvcoords.size();
    const size_t numNormals = mesh.normals.size();

    const GLsizei numBuffers = (numVertices > 0) + (numUvCoords > 0) + (numNormals > 0);
    if(numBuffers > 0)
    {
        GLuint bufferIds[3] = {};
        glGenBuffers(numBuffers, bufferIds);

        size_t bufferIdsIndex = 0;
        glMesh.numVertices = numVertices;
        if (numVertices > 0)
        {
            const GLuint bufferId = bufferIds[bufferIdsIndex++];
            glMesh.vertexBufferId = bufferId;

            glBindBuffer(GL_ARRAY_BUFFER, bufferId);
            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
        }
        if (numUvCoords > 0)
        {
            const GLuint bufferId = bufferIds[bufferIdsIndex++];
            glMesh.uvBufferId = bufferId;

            glBindBuffer(GL_ARRAY_BUFFER, bufferId);
            glBufferData(GL_ARRAY_BUFFER, numUvCoords * sizeof(glm::vec2), mesh.uvcoords.data(), GL_STATIC_DRAW);
        }
        if (numNormals > 0)
        {
            const GLuint bufferId = bufferIds[bufferIdsIndex];
            glMesh.normalBufferId = bufferId;

            glBindBuffer(GL_ARRAY_BUFFER, bufferId);
            glBufferData(GL_ARRAY_BUFFER, numNormals * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    return glMesh;
}
