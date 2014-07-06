
#include "ModelLoader_OBJ.h"
#include <glm/glm.hpp>
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <regex>
#include <limits>

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

std::istream& operator>>(std::istream& stream, const char* str)
{
    const char* it = str;
    while (*it != '\0')
    {
        char next;
        stream.get(next);
        if (next != *it)
        {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        ++it;
    }
    return stream;
}

void ParseOBJ(const char *filename,
              std::vector<glm::vec3>& verticies,
              std::vector<glm::vec2>& uvcoords,
              std::vector<glm::vec3>& normals)
{
    using namespace std;
    deque<glm::vec3> verticies_temp;
    deque<glm::vec2> uvs_temp;
    deque<glm::vec3> normals_temp;

    FaceType faceType = Unclassified;
    ifstream file(filename);
    string line_type;

    while (file >> line_type)
    {
        if (line_type == "v")
        {
            glm::vec3 vertex;
            file >> vertex.x >> vertex.y >> vertex.z;
            verticies_temp.emplace_back(vertex);
        }
        else if (line_type == "vt")
        {
            glm::vec2 uv;
            file >> uv.x >> uv.y;
            uvs_temp.emplace_back(uv);
        }
        else if (line_type == "vn")
        {
            glm::vec3 normal;
            file >> normal.x >> normal.y >> normal.z;
            normals_temp.emplace_back(normal);
        }
        else if (line_type == "f")
        {
            if (faceType == Unclassified)
            {
                string face;
                getline(file, face);
                // tellg and seekg are stupid, back up using this instead
                for (auto c : face)
                {
                    file.unget();
                }
                faceType = identifyFace(face);
            }

            verticies.reserve(verticies_temp.size());
            uvcoords.reserve(uvs_temp.size());
            normals.reserve(normals_temp.size());

            uint verts[3] = {};
            uint uvs[3] = {};
            uint norms[3] = {};
            if (faceType == PosUvNorm)
            {
                file >> verts[0] >> "/" >> uvs[0] >> "/" >> norms[0]
                     >> verts[1] >> "/" >> uvs[1] >> "/" >> norms[1]
                     >> verts[2] >> "/" >> uvs[2] >> "/" >> norms[2];
            }
            else if (faceType == PosNorm)
            {
                file >> verts[0] >> "//" >> norms[0]
                     >> verts[1] >> "//" >> norms[1]
                     >> verts[2] >> "//" >> norms[2];
            }
            else if (faceType == PosUv)
            {
                file >> verts[0] >> "/" >> uvs[0]
                     >> verts[1] >> "/" >> uvs[1]
                     >> verts[2] >> "/" >> uvs[2];
            }
            else // faceType == Pos
            {
                file >> verts[0] >> verts[1] >> verts[2];
            }

            if (verts[0] != 0)
            {
                verticies.emplace_back(verticies_temp[verts[0] - 1]);
                verticies.emplace_back(verticies_temp[verts[1] - 1]);
                verticies.emplace_back(verticies_temp[verts[2] - 1]);
            }
            if (norms[0] != 0)
            {
                normals.emplace_back(normals_temp[norms[0] - 1]);
                normals.emplace_back(normals_temp[norms[1] - 1]);
                normals.emplace_back(normals_temp[norms[2] - 1]);
            }
            if (uvs[0] != 0)
            {
                uvcoords.emplace_back(uvs_temp[uvs[0] - 1]);
                uvcoords.emplace_back(uvs_temp[uvs[1] - 1]);
                uvcoords.emplace_back(uvs_temp[uvs[2] - 1]);
            }
        }
        file.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

Mesh LoadMeshFromOBJ(const char *filename)
{
    Mesh mesh;
    ParseOBJ(filename, mesh.verticies, mesh.uvcoords, mesh.normals);
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
