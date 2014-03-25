
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <limits>

std::vector<glm::vec3> ParseOBJ(const char *filename)
{
    std::vector<glm::vec3> verticies;

    std::ifstream file(filename);
    std::string line_type;

    while (file >> line_type)
    {
        if (line_type == "v")
        {
            glm::vec3 vertex;
            file >> vertex.x >> vertex.y >> vertex.z;
            verticies.emplace_back(vertex);
        }
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return verticies;
}
