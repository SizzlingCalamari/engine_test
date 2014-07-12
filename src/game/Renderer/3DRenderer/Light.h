
#pragma once

#include <glm/vec3.hpp>

struct BaseLight
{
    glm::vec3 colour = glm::vec3(0.0f);
    float ambientIntensity = 0.0f;
    float diffuseIntensity = 0.0f;
};

struct DirectionalLight: public BaseLight
{
    glm::vec3 direction = glm::vec3(0.0f);
};

struct PointLight: public BaseLight
{
    PointLight():
        position(glm::vec3(0.0f)),
        attenuation({ 1.0f, 0.0f, 0.0f })
    {
    }

    glm::vec3 position = glm::vec3(0.0f);
    struct
    {
        float constant;
        float linear;
        float exp;
    } attenuation;
};
