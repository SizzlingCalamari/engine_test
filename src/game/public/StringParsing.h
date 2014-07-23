
#pragma once

#include <glm/vec3.hpp>
#include <sstream>
#include <string>

template<typename T>
inline bool StringTo(const std::string& str, T& out)
{
    std::stringstream ss(str);
    return !!(ss >> out);
}

// format "%f %f %f"
template<>
inline bool StringTo(const std::string& str, glm::vec3& out)
{
    std::stringstream ss(str);
    return !!(ss >> out.x >> out.y >> out.z);
}
