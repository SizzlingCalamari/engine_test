
#pragma once

#include "ModelParser_OBJ.h"
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <limits>

std::vector<glm::vec3> ParseOBJ(const char *filename)
{
    using namespace std;
    vector<glm::vec3> verticies;
    vector<glm::vec3> verticies_temp;
    vector<glm::vec2> uvs_temp;
    vector<glm::vec3> normals_temp;

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
            uint verts[3] = {};
            file >> verts[0] >> verts[1] >> verts[2];
            verticies.emplace_back(verticies_temp[verts[0] - 1]);
            verticies.emplace_back(verticies_temp[verts[1] - 1]);
            verticies.emplace_back(verticies_temp[verts[2] - 1]);
        }
        file.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return verticies;
}

Mesh LoadMeshFromOBJ(const char *filename)
{
    Mesh mesh;
    auto verts = ParseOBJ(filename);
    mesh.LoadVerticies(verts.data(), sizeof(glm::vec3), verts.size());
    return mesh;
}
