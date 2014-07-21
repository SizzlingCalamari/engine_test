
#include "ModelLoader_OBJ.h"
#include "utils.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <deque>
#include <cstdio>
#include <string>
#include <regex>

enum FaceType
{
    Unclassified = 0,
    PosUvNorm,
    PosNorm,
    PosUv,
    Pos
};

FaceType identifyFace(const std::string& face)
{
    std::regex posUvNorm("\\d+/\\d+/\\d+");
    if (std::regex_search(face, posUvNorm))
    {
        return PosUvNorm;
    }

    std::regex posNorm("\\d+//\\d+");
    if (std::regex_search(face, posNorm))
    {
        return PosNorm;
    }

    std::regex posUv("\\d+/\\d+");
    if (std::regex_search(face, posUv))
    {
        return PosUv;
    }

    return Pos;
}

inline void SkipToNextLine(FILE* file)
{
    char c;
    while ((c = fgetc(file)) != EOF && c != '\n');
}

inline void UpdateAABB(const glm::vec3& vertex,
                       glm::vec3& minAABB, glm::vec3& maxAABB)
{
    minAABB.x = utils::min(vertex.x, minAABB.x);
    minAABB.y = utils::min(vertex.y, minAABB.y);
    minAABB.z = utils::min(vertex.z, minAABB.z);

    maxAABB.x = utils::max(vertex.x, maxAABB.x);
    maxAABB.y = utils::max(vertex.y, maxAABB.y);
    maxAABB.z = utils::max(vertex.z, maxAABB.z);
}

void ParseOBJ(const char *filename,
              std::vector<glm::vec3>& verticies,
              std::vector<glm::vec2>& uvcoords,
              std::vector<glm::vec3>& normals,
              glm::vec3& minAABB,
              glm::vec3& maxAABB)
{
    minAABB = glm::vec3();
    maxAABB = glm::vec3();

    std::deque<glm::vec3> verticies_temp;
    std::deque<glm::vec2> uvs_temp;
    std::deque<glm::vec3> normals_temp;

    auto *file = fopen(filename, "r");
    if (!file)
    {
        return;
    }

    char firstOnLine, secondOnLine;
    FaceType faceType = Unclassified;

    while ((firstOnLine = fgetc(file)) != EOF)
    {
        if (firstOnLine == '\n')
        {
            continue;
        }
        if ((secondOnLine = fgetc(file)) == EOF)
        {
            break;
        }

        if (firstOnLine == 'v' && secondOnLine == ' ')
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            UpdateAABB(vertex, minAABB, maxAABB);
            verticies_temp.emplace_back(vertex);
        }
        else if (firstOnLine == 'v' && secondOnLine == 't')
        {
            glm::vec2 uv;
            fscanf(file, "%f %f", &uv.x, &uv.y);
            uvs_temp.emplace_back(uv);
        }
        else if (firstOnLine == 'v' && secondOnLine == 'n')
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f", &normal.x, &normal.y, &normal.z);
            normals_temp.emplace_back(normal);
        }
        else if (firstOnLine == 'f')
        {
            if (faceType == Unclassified)
            {
                std::string face(64, '\0');
                fgets(&face[0], 64, file);
                int length = static_cast<int>(strlen(face.c_str()));
                // back up to start of line
                for (int i = length-1; i >= 0; --i)
                {
                    ungetc(face[i], file);
                }
                faceType = identifyFace(face);
            }

            uint verts[3] = {};
            uint uvs[3] = {};
            uint norms[3] = {};
            if (faceType == PosUvNorm)
            {
                fscanf(file, "%i/%i/%i %i/%i/%i %i/%i/%i",
                       &verts[0], &uvs[0], &norms[0],
                       &verts[1], &uvs[1], &norms[1],
                       &verts[2], &uvs[2], &norms[2]);
            }
            else if (faceType == PosNorm)
            {
                fscanf(file, "%i//%i %i//%i %i//%i",
                       &verts[0], &norms[0],
                       &verts[1], &norms[1],
                       &verts[2], &norms[2]);
            }
            else if (faceType == PosUv)
            {
                fscanf(file, "%i/%i %i/%i %i/%i",
                       &verts[0], &uvs[0],
                       &verts[1], &uvs[1],
                       &verts[2], &uvs[2]);
            }
            else // faceType == Pos
            {
                fscanf(file, "%i %i %i", &verts[0], &verts[1], &verts[2]);
            }

            if (verts[0] != 0)
            {
                verticies.reserve(verticies_temp.size());
                verticies.emplace_back(verticies_temp[verts[0] - 1]);
                verticies.emplace_back(verticies_temp[verts[1] - 1]);
                verticies.emplace_back(verticies_temp[verts[2] - 1]);
            }
            if (norms[0] != 0)
            {
                normals.reserve(normals_temp.size());
                normals.emplace_back(normals_temp[norms[0] - 1]);
                normals.emplace_back(normals_temp[norms[1] - 1]);
                normals.emplace_back(normals_temp[norms[2] - 1]);
            }
            if (uvs[0] != 0)
            {
                uvcoords.reserve(uvs_temp.size());
                uvcoords.emplace_back(uvs_temp[uvs[0] - 1]);
                uvcoords.emplace_back(uvs_temp[uvs[1] - 1]);
                uvcoords.emplace_back(uvs_temp[uvs[2] - 1]);
            }
        }
        SkipToNextLine(file);
    }
    fclose(file);
}

Mesh LoadMeshFromOBJ(const char *filename)
{
    Mesh mesh;
    ParseOBJ(filename, mesh.verticies, mesh.uvcoords, mesh.normals, mesh.minAABB, mesh.maxAABB);
    mesh.numVerticies = mesh.verticies.size();
    if (mesh.numVerticies > 0)
    {
        mesh.vertexBufferId = LoadVerticies(mesh.verticies.data(), sizeof(glm::vec3), mesh.numVerticies);
    }
    if (mesh.uvcoords.size() > 0)
    {
        mesh.uvBufferId = LoadVerticies(mesh.uvcoords.data(), sizeof(glm::vec2), mesh.uvcoords.size());
    }
    if (mesh.normals.size() > 0)
    {
        mesh.normalBufferId = LoadVerticies(mesh.normals.data(), sizeof(glm::vec3), mesh.normals.size());
    }
    
    return mesh;
}
